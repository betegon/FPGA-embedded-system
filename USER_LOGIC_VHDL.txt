library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;

entity user_logic is
  generic
  (
    C_SLV_DWIDTH                   : integer              := 32;
    C_NUM_REG                      : integer              := 4
  );
  port
  (
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Reset                   : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(0 to C_SLV_DWIDTH-1);
    Bus2IP_BE                      : in  std_logic_vector(0 to C_SLV_DWIDTH/8-1);
    Bus2IP_RdCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    Bus2IP_WrCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    IP2Bus_Data                    : out std_logic_vector(0 to C_SLV_DWIDTH-1);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;

  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Reset  : signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

  --USER signal declarations added here, as needed for user logic

  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal slv_reg0                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg1                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg2                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg3                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg_write_sel              : std_logic_vector(0 to 3);
  signal slv_reg_read_sel               : std_logic_vector(0 to 3);
  signal slv_ip2bus_data                : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_read_ack                   : std_logic;
  signal slv_write_ack                  : std_logic;
  type state_type is (init,waitt,add,disp);
  signal state: state_type;
  signal tot: unsigned(7 downto 0);
  signal   C:  std_logic; -- Coin inserted
  signal    A:  unsigned(7 downto 0); -- Value of inserted coin 
  signal    S:  unsigned(7 downto 0); -- Value of soda product
  signal    D:  std_logic ;-- Dispens the product
  signal resto: unsigned (7 downto 0);

begin

  
  A<=unsigned(slv_reg0(24 to 31));-- INSERT - valor moneda
  S<=unsigned(slv_reg1(24 to 31));-- VALOR - valor bebida
  slv_reg2(24 to 31) <= std_logic_vector(tot);-- TOTAL - total de dinero metido
  slv_reg3(24 to 31) <= std_logic_vector(resto);-- RESTO - resto de dinero que queda para llegar al valor de la bebida
  slv_reg2(0 to 23)<=(others=>'0');
  slv_reg3(0 to 23)<=(others=>'0');
  
  main_synch_process: process(Bus2IP_Clk,Bus2IP_Reset )
    begin
      if Bus2IP_Reset='1' then
        state<=init;
        tot<=(others=>'0');
		  resto<=(others=>'1');
      
		elsif Bus2IP_Clk='1' and Bus2IP_Clk'event then
        case state is
		  
        when init =>    
          state<=waitt;
          tot<=(others=>'0');
			 
        when waitt =>
		    if C='1' then
            state<=add;
          else
            if tot>=S then
              state<=disp;
            end if;          
          end if;
        
		  when add =>
          tot<=tot+A;
			 resto<=S-tot;
          state<=waitt;
        
		  when disp =>
		    resto<=(others=>'0');
          state<=init;
        end case;
      end if;
    end process;
    
    output_logic: process(state)
    begin
      case state is
      
		when disp =>
        D<='1';
      
		when others =>
        D<='0';
      end case;
    end process;

  slv_reg_write_sel <= Bus2IP_WrCE(0 to 3);
  slv_reg_write_sel <= Bus2IP_WrCE(0 to 3);
  slv_reg_read_sel  <= Bus2IP_RdCE(0 to 3);
  slv_write_ack     <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2) or Bus2IP_WrCE(3) or Bus2IP_WrCE(4);
  slv_read_ack      <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2) or Bus2IP_RdCE(3) or Bus2IP_RdCE(4);

  SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
  begin


    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      if Bus2IP_Reset = '1' then
        slv_reg0 <= (others => '0');
        slv_reg1 <= (others => '0');
		  C <= '0';
      else
		  C <= '0';

        case slv_reg_write_sel is
          when "1000" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
					 C<='1';
                slv_reg0(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
				 end if;
            end loop;
          when "0100" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
					 C<= '0';
                slv_reg1(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
				 end loop;

          when others => C<= '0';
        end case;
      end if;
    end if;

  end process SLAVE_REG_WRITE_PROC;
 
  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg0, slv_reg1, slv_reg2, slv_reg3) is
  begin

    case slv_reg_read_sel is
      when "1000" => slv_ip2bus_data <= slv_reg0;
      when "0100" => slv_ip2bus_data <= slv_reg1;
      when "0010" => slv_ip2bus_data <= slv_reg2;
      when "0001" => slv_ip2bus_data <= slv_reg3;
      when others => slv_ip2bus_data <= (others => '0');
    end case;

  end process SLAVE_REG_READ_PROC;

  IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
                  (others => '0');

  IP2Bus_WrAck <= slv_write_ack;
  IP2Bus_RdAck <= slv_read_ack;
  IP2Bus_Error <= '0';

end IMP;
