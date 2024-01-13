#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "Xenon/Core/Bus/Bus.h"
#include "Xenon/Core/PostBus/PostBus.h"
#include "Xenon/Core/NAND/NAND.h"
#include "Xenon/Core/XCPU/XCPU.h"
#include "Xenon/Core/RAM/RAM.h"
#include "Xenon/Core/UART/UART.h"

int main(int argc, char* argv[])
{
	Bus Bus;
	PostBus postBus;
	NAND nandDevice;
	RAM ram;
	UART uart;

	Bus.Init();

	postBus.Initialize("PostBus", POST_BUS_ADDR, POST_BUS_ADDR);
	nandDevice.Initialize("NAND", NAND_START_ADDR, NAND_END_ADDR);
	ram.Initialize("RAM", RAM_START_ADDR, RAM_START_ADDR + RAM_SIZE);
	uart.Initialize("UART", UART_START_ADDR, UART_END_ADDR);

	Bus.AddDevice(&postBus);
	Bus.AddDevice(&nandDevice);
	Bus.AddDevice(&ram);
	Bus.AddDevice(&uart);
    nandDevice.Load("C://Xbox/jasper_nand.bin");
    Xe::Core::XCPU::XCPU xcpu("C://Xbox/1bl.bin", &Bus);
    
    xcpu.Start(0x8000020000000100);
	return 0;
}