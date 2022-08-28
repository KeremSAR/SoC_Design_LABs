--
-- EEM464 SoC Lab 
--
-- Description: Top level controller that controls the OLED display.
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity mod_mul_with_oled is
generic(
    constant R : integer :=8
    );
    port (  clk         : in std_logic;
            rst         : in std_logic;
            start         : in std_logic;
            sw_in       : in std_logic_vector(7 downto 0);
            en          : in std_logic; 
            oled_sdin   : out std_logic;
            oled_sclk   : out std_logic;
            oled_dc     : out std_logic;
            oled_res    : out std_logic;
            oled_vbat   : out std_logic;
            oled_vdd    : out std_logic);
end mod_mul_with_oled;

architecture behavioral of mod_mul_with_oled is

component ModPro is port(
	a_mon : in unsigned(R-1 downto 0);
	b_mon : in unsigned(R-1 downto 0);
	clk: in std_logic;
	reset: in std_logic; 
	start: in std_logic;
	z: out unsigned(R-1 downto 0);
	done1: out std_logic);
 end component;

    component debounce IS
  GENERIC(
    counter_size  :  INTEGER := 20); --counter size (19 bits gives 10.5ms with 50MHz clock)
  PORT(
    clk     : IN  STD_LOGIC;  --input clock
    button  : IN  STD_LOGIC;  --input signal to be debounced
    result  : OUT STD_LOGIC); --debounced signal
END component;

    component oled_init is
        port (  clk         : in std_logic;
                rst         : in std_logic;
                en          : in std_logic;
                sdout       : out std_logic;
                oled_sclk   : out std_logic;
                oled_dc     : out std_logic;
                oled_res    : out std_logic;
                oled_vbat   : out std_logic;
                oled_vdd    : out std_logic;
                fin         : out std_logic);
    end component;

    component oled_drive is
        port (  clk         : in std_logic;
                rst         : in std_logic;
                en          : in std_logic;
                a_reg       : in std_logic_vector(15 downto 0);
                b_reg       : in std_logic_vector(15 downto 0);
                n_reg       : in std_logic_vector(15 downto 0);
                z_reg       : in std_logic_vector(15 downto 0);
                sdout       : out std_logic;
                oled_sclk   : out std_logic;
                oled_dc     : out std_logic;
                fin         : out std_logic);
    end component;

    type states is (Idle, OledInitialize, LoadA_1, LoadB_1, OledExample, WaitMult,Done);

    signal current_state, next_state : states := Idle;

    
    signal init_en          : std_logic := '0';
    signal init_done        : std_logic;
    signal init_sdata       : std_logic;
    signal init_spi_clk     : std_logic;
    signal init_dc          : std_logic;

    signal example_en       : std_logic := '0';
    signal example_sdata    : std_logic;
    signal example_spi_clk  : std_logic;
    signal example_dc       : std_logic;
    signal example_done     : std_logic;
    
    signal a_reg : std_logic_vector(15 downto 0) := (others => '0');
    signal b_reg : std_logic_vector(15 downto 0) := (others => '0');
    signal n_reg : std_logic_vector(15 downto 0) := (others => '0');
    signal z_reg : std_logic_vector(15 downto 0) := (others => '0');
    signal ready_mult : std_logic;
    signal start_mult : std_logic := '0';
    signal n  :unsigned(R-1 downto 0) := "11101001";
    
    signal A_value : std_logic_vector(R-1 downto 0):= (others => '0');
    signal B_value : std_logic_vector(R-1 downto 0):= (others => '0');
    signal N_value : std_logic_vector(R-1 downto 0):= (others => '0');
    signal Z_value : std_logic_vector(R-1 downto 0):= (others => '0');
                    
	signal en_db : std_logic;
	
	 signal temp_z,temp_z2: unsigned(R-1 downto 0);
	  signal done1,done2 : std_logic;
	--signal t_mult : unsigned(31 downto 0);
	--signal t : unsigned(15 downto 0);
	--signal a_mon, b_mon :unsigned(15 downto 0); 
	--signal m_mult : unsigned(31 downto 0);
	--signal m : unsigned(15 downto 0);

	--signal m_n_mult     : unsigned(31 downto 0);
	--signal u  : unsigned(32 downto 0);
	--signal u_divided     : unsigned(16 downto 0);
	--signal x_prime          : unsigned(16 downto 0);
	--signal u2        : unsigned(31 downto 0);
	--signal u2_divided       : unsigned(15 downto 0);
  --  signal x          : unsigned(15 downto 0);
begin
	MnPro: ModPro port map (unsigned(A_value),unsigned(B_value),clk,rst,start,temp_z,done1);
	MnPro2: ModPro port map (temp_z, "00000001",clk,rst,done1,temp_z2,done2); 
	
    Debouncing: debounce port map(clk, en, en_db);

    Initialize: oled_init port map (clk,
                                    rst,
                                    init_en,
                                    init_sdata,
                                    init_spi_clk,
                                    init_dc,
                                    oled_res,
                                    oled_vbat,
                                    oled_vdd,
                                    init_done);

    Drive_OLED_Screen: oled_drive port map ( clk,
                                            rst,
                                            example_en,
                                            a_reg,
                                            b_reg,
                                            n_reg,
                                            z_reg,
                                            example_sdata,
                                            example_spi_clk,
                                            example_dc,
                                            example_done);

    -- MUXes to indicate which outputs are routed out depending on which block is enabled
    oled_sdin <= init_sdata when current_state = OledInitialize else example_sdata;
    oled_sclk <= init_spi_clk when current_state = OledInitialize else example_spi_clk;
    oled_dc <= init_dc when current_state = OledInitialize else example_dc;
    -- End output MUXes

    -- MUXes that enable blocks when in the proper states
    init_en <= '1' when current_state = OledInitialize else '0';
    example_en <= '1' when current_state = OledExample  else
				--  '1' when current_state = LoadA_0 else
                  '1' when current_state = LoadA_1 else
                  --'1' when current_state = LoadB_0 else
                  '1' when current_state = LoadB_1 else
				  '1' when current_state = WaitMult else
                  '1' when current_state = Done else '0';
    -- End enable MUXes
    a_reg <= "00000000" & A_value;
    b_reg <= "00000000" & B_value;
    Z_value <= std_logic_vector(temp_z2);
    z_reg <= "00000000" & Z_value;
    N_value<= std_logic_vector(n);
	n_reg <= "00000000"&N_value;
	--a_mon <= unsigned(a_reg);
	--b_mon <= unsigned(b_reg);
    -- z_reg <= std_logic_vector(temp_z2);
    process (clk)
    begin
        if rising_edge(clk) then
		
            if rst = '1' then
                current_state <= Idle;
                start_mult <= '0';
            else
                case current_state is
                    when Idle =>
                        current_state <= OledInitialize;
                    -- Go through the initialization sequence
                    when OledInitialize =>
                        if init_done = '1' then
                           current_state <= OledExample;
                           next_state <= LoadA_1;
                        end if;
                        
                    -- Do example and do nothing when finished
                    when OledExample =>
                        if example_done = '1' then
                            current_state <= next_state;
                        end if; 
					--when LoadA_0 =>
						--if en_db ='1' then
							--A_value(15 downto 8) <= sw_in;
							--current_state <= OledExample;
							--next_state <= LoadA_1;
						--else
							--next_state <= LoadA_0;
						--end if;
					when LoadA_1 =>
						if en_db = '1' then
							A_value(7 downto 0) <= sw_in;
							current_state <= OledExample;
							next_state <= LoadB_1;
						else
							next_state <= LoadA_1;
						end if;
					--when LoadB_0 =>
						--if en_db = '1' then
							--B_value(15 downto 8) <= sw_in;
							--current_state <= OledExample;
							--next_state <= LoadB_1;
						--else
							--next_state <= LoadB_0;
						--end if;
					when LoadB_1 =>
						if en_db = '1' then
							B_value(7 downto 0) <= sw_in;
							current_state <= OledExample;
							next_state <= WaitMult;
						else
							next_state <= LoadB_1;
						end if;
					
					 when WaitMult =>     
                        if start = '1' then
                            current_state <= OledExample;
                            next_state <= Done;          
                        else
                            next_state <= WaitMult;
                        end if;	
                    
                    --
                    -- Necessary states should be incorporated to this FSM by YOU.
                    --
                    
                    
                    --------------------------------------------------------------------------------
                    -- You can add as many states as you wish in order to achieve working design. --                    
                    --------------------------------------------------------------------------------
                    
                    
                    
                    -- Do nothing
                    when Done =>
                        current_state <= Done;
                    when others =>
                       current_state <= Idle;
                end case;
            end if;
        end if;
    end process;
    
    
    
     --------------------------------------------------------------
     -- You also need to instantiate ModMult.vhd component here. --
     --------------------------------------------------------------
  
    -- start_mult is required to start the operation of the modular multiplication
    -- ready_mult is control signal which indicates that the modulo multiplication completes its operation.
    -- z_reg is the result value taken from your Modulo Multiplier hardware IP core.
    -- All other parameters required for the multiplier circuit might be a constant in your ModMult.vhd file.


end behavioral;