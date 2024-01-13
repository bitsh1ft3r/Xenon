#pragma once

#include <string>
#include <vector>
#include <assert.h>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"
#include "Xenon/Base/SystemDevice.h"
#include "Xenon/Core/XCPU/XCPU_SPRs.h"
#include "Xenon/Core/XCPU/PowerPC.h"
#include "Xenon/Core/XCPU/Interpreter/PowerPCState.h"



namespace Xe
{
	namespace Core 
	{
		namespace XCPU
		{
			class XCPU : public SystemDevice
			{
			public:
				XCPU(std::string blPath, Bus *busPointer);
				void Start(u64 startAddress);
			private:

				// CPU Context
				XCPUContext cpuContext;

				bool Load1BL(std::string filePath);

				//
				// PowerPC CPU State
				//

				PowerPCState cpuState;			
			};
		}
	}
}