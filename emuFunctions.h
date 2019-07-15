void noop(){
	std::cout << "No Operation at " << prgCounter << std::endl;
	std::cout << "Opcode:" << std::hex << operation << std::endl;
	killSwitch = true;
}

void opcode0x00E0() {
	std::fill(screen.begin(), screen.end(), 0);
	drawn = true;
}

void opcode0x00EE() {
	--stackPointer;
	prgCounter = stack[stackPointer];
}

void (*opcode0x0000[3])() = {
	opcode0x00E0, opcode0x00EE, noop
};

void chipSpecial(){
	opcode0x0000[(operation & 0x00FF)]();
}

void opcode0x1000() {
	prgCounter = operation & 0x0FFF;
}

void opcode0x2000() {
	stack[stackPointer] = prgCounter;
	++stackPointer;
	prgCounter = operation & 0x0FFF;
}

void opcode0x3000() {
	if (registers[(operation & 0x0F00) >> 8] == (operation & 0x00FF))
		prgCounter += 2;	
}

void opcode0x4000() {
	if (registers[(operation & 0x0F00) >> 8] != (operation & 0x00FF))
		prgCounter += 2;
}

void opcode0x5000() {
	if (registers[(operation & 0x0F00) >> 8] == registers[(operation & 0x00F0) >> 4])
		prgCounter += 2;	
}

void opcode0x6000() {
	registers[(operation & 0x0F00) >> 8] = operation & 0x00FF;
	
}

void opcode0x7000() {
	registers[(operation & 0x0F00) >> 8] += operation & 0x00FF;
	
}

void opcode0x8XY0() {
	registers[(operation & 0x0F00) >> 8] = registers[(operation & 0x00F0) >> 4];
	
}

void opcode0x8XY1() {
	registers[(operation & 0x0F00) >> 8] |= registers[(operation & 0x00F0) >> 4];
	
}

void opcode0x8XY2() {
	registers[(operation & 0x0F00) >> 8] &= registers[(operation & 0x00F0) >> 4];
	
}

void opcode0x8XY3() {
	registers[(operation & 0x0F00) >> 8] &= registers[(operation & 0x00F0) >> 4];
	
}

void opcode0x8XY4() {
	registers[(operation & 0x0F00) >> 8] += registers[(operation & 0x00F0) >> 4];
	if (registers[(operation & 0x00F0) >> 4] > (0xFF - registers[(operation & 0x0F00) >> 8]))
		registers[0xF] = 1;
	else
		registers[0xF] = 0;
	
}

void opcode0x8XY5() {
	if (registers[(operation & 0x00F0) >> 4] > registers[(operation & 0x0F00) >> 8])
		registers[0xF] = 0;
	else
		registers[0xF] = 1;
	registers[(operation & 0x0F00) >> 8] -= registers[(operation & 0x00F0) >> 4];
	
}

void opcode0x8XY6() {
	registers[0xF] = registers[(operation & 0x0F00) >> 8] & 0x1;
	registers[(operation & 0x0F00) >> 8] >>= 1;
	
}

void opcode0x8XY7() {
	if (registers[(operation & 0x0F00) >> 8] > registers[(operation & 0x00F0) >> 4])
		registers[0xF] = 0;
	else
		registers[0xF] = 1;
	registers[(operation & 0x0F00) >> 8] = registers[(operation & 0x00F0) >> 4] - registers[(operation & 0x0F00) >> 8];
	
}

void opcode0x8XYE() {
	registers[0xF] = registers[(operation & 0x0F00) >> 8] >> 7;
	registers[(operation & 0x0F00) >> 8] <<= 1;
	
}

void (*opcode0x8000[10])() = {
	opcode0x8XY0, opcode0x8XY1, opcode0x8XY2, opcode0x8XY3, opcode0x8XY4,
	opcode0x8XY5, opcode0x8XY6, opcode0x8XY7, opcode0x8XYE, noop
};

void chipMath(){
	opcode0x8000[(operation & 0x000F)]();
}

void opcode0x9000() {
	if (registers[(operation & 0x0F00) >> 8] != registers[(operation & 0x00F0) >> 4])
		prgCounter += 2;
}

void opcode0xA000() {
	indexRegister = operation & 0x0FFF;
	
}

void opcode0xB000() {
	prgCounter = (operation & 0x0FFF) + registers[0];
}

void opcode0xC000() {
	registers[(operation & 0x0F00) >> 8] = std::uniform_int_distribution<>(0, 255)(rnd) & (operation & 0x00FF);
	
}

void opcode0xD000() {
	uint16_t x = registers[(operation & 0x0F00) >> 8];
	uint16_t y = registers[(operation & 0x00F0) >> 4];
	uint16_t height = operation & 0x000F;
	uint16_t pixel;

	registers[0xF] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = mem[indexRegister + yline];
		for (int xline = 0; xline < 8; xline++)
		{
			if ((pixel & (0x80 >> xline)) != 0)
			{
				if (screen[(x + xline + ((y + yline) * 64))] == 1)
				{
					registers[0xF] = 1;
				}
				screen[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	drawn = true;
	
}

void opcode0xE09E() {
	if (keys[registers[(operation & 0x0F00) >> 8]] != 0)
		prgCounter += 2;
}

void opcode0xE0A1() {
	if (keys[registers[(operation & 0x0F00) >> 8]] == 0)
		prgCounter += 2;	
}

void (*opcode0xE000[3])() =  {
	opcode0xE09E, opcode0xE0A1, noop
};

void chipInput(){
	opcode0xE000[(operation & 0x00FF)]();
}

void opcode0xF007() {
	registers[(operation & 0x0F00) >> 8] = delayTime;
	
}

void opcode0xF00A() {
	bool keyPress = false;

	for (int i = 0; i < 16; ++i)
	{
		if (keys[i] != 0)
		{
			registers[(operation & 0x0F00) >> 8] = i;
			keyPress = true;
		}
	}

	//Come on! Just press the key! :(
	if (!keyPress)
		return;

	
}

void opcode0xF015() {
	delayTime = registers[(operation & 0x0F00) >> 8];
	
}

void opcode0xF018() {
	soundTime = registers[(operation & 0x0F00) >> 8];
	
}

void opcode0xF01E() {
	if (indexRegister + registers[(operation & 0x0F00) >> 8] > 0xFFF)
		registers[0xF] = 1;
	else
		registers[0xF] = 0;
	indexRegister += registers[(operation & 0x0F00) >> 8];
	
}

void opcode0xF029() {
	indexRegister = registers[(operation & 0x0F00) >> 8] * 0x5;
	
}

void opcode0xF033() {
	mem[indexRegister] = registers[(operation & 0x0F00) >> 8] / 100;
	mem[indexRegister + 1] = (registers[(operation & 0x0F00) >> 8] / 10) % 10;
	mem[indexRegister + 2] = registers[(operation & 0x0F00) >> 8] % 10;
	
}

void opcode0xF055() {
	for (int i = 0; i <= ((operation & 0x0F00) >> 8); ++i)
		mem[indexRegister + i] = registers[i];

	indexRegister += ((operation & 0x0F00) >> 8) + 1;
	
}

void opcode0xF065() {
	for (int i = 0; i <= ((operation & 0x0F00) >> 8); ++i)
		registers[i] = mem[indexRegister + i];

	indexRegister += ((operation & 0x0F00) >> 8) + 1;
	
}

void (*opcode0xF000[10])() = {
	opcode0xF007,
	opcode0xF00A,
	opcode0xF015,
	opcode0xF018,
	opcode0xF01E,
	opcode0xF029,
	opcode0xF033,
	opcode0xF055,
	opcode0xF065,
	noop
};

void chipTechnical(){
	opcode0xF000[(operation & 0x00FF)]();
}

void (*Chip8Table[17])() = {
		chipSpecial,
		opcode0x1000,
		opcode0x2000,
		opcode0x3000,
		opcode0x4000,
		opcode0x5000,
		opcode0x6000,
		opcode0x7000,
		chipMath,
		opcode0x9000,
		opcode0xA000,
		opcode0xB000,
		opcode0xC000,
		opcode0xD000,
		chipInput,
		chipTechnical,
		noop
};