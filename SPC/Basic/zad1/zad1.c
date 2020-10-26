#include <LPC213X.H>
void DAC_MCP_4921_Set(unsigned int uiVoltage) {
	IO0SET = 010101 << 8;
	IO0CLR = 101010 << 8;
	IO0DIR = 1 << 20;
}

int main() {
}
