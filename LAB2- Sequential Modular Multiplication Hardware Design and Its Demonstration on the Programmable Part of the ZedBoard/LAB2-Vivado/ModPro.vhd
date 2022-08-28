-- Kerem SARI--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity ModPro is
    generic(
    constant R : integer :=8
    );
		port (  a_mon : in unsigned(R-1 downto 0);
				b_mon : in unsigned(R-1 downto 0);
				clk: in std_logic;
				reset: in std_logic; 
				start: in std_logic;
				z: out unsigned(R-1 downto 0);
				done1: out std_logic);
end ModPro;

architecture behavioral of ModPro is

signal t_mult, m_mult, m_n_mult: unsigned(2*R-1 downto 0);
signal t,m : unsigned(R-1 downto 0);
signal u  : unsigned(2*R downto 0);
signal u_divided     : unsigned(R downto 0);
--signal answer : unsigned(16 downto 0);	
signal n     : unsigned(R-1 downto 0) := "11101001";--121      
signal n_prime : unsigned(R-1 downto 0) := "01011001"; --  -55 -> 55  8bit
--signal n_prime : unsigned(R-1 downto 0) := "0110100111001001"; -- 27081  16bit 

begin
	process(clk,reset,start)
	begin
		if rising_edge(clk) then
			if reset = '1' then
				 z <= (others =>'0');
				 done1 <= '0';
			elsif start ='0' then
				 done1<='0';
			else
				t_mult <= (a_mon* b_mon );
				t <= t_mult(R-1 downto 0);
				m_mult <= (t * n_prime);
				m <= m_mult(R-1 downto 0);
				m_n_mult <= (m * n);
				u <= ('0'& m_n_mult)+ ('0'& t_mult);
				u_divided <= u(2*R downto R);
				done1 <= '1';
				if(not(u_divided<n)) then
					z <= (u_divided(R-1 downto 0) - n);				
				else
					z <= u_divided(R-1 downto 0);
					
				end if;
			end if;
		end if;
	end process;
end behavioral;		
				