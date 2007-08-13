//
// C++ Implementation: sore_resource_testing
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <ctime>
#include <iostream>
#include <sore.h>
//#include "allgl.h"

//using namespace SORE_FileIO;
//using namespace SORE_Resource;
using namespace std;

int main();

int main()
{
	//SORE_FileIO::Init();
	//SORE_FileIO::CachePackageInfo("data_compressed.sdp");
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->RegisterDataLoader((SORE_Resource::RES_LOAD_DATA)SORE_Resource::LoadDataBuffer, "buf");
	rm->RegisterLoader((SORE_Resource::RES_LOAD)SORE_Resource::LoadTexture, "tga");
	SORE_Resource::res_handle buf1 = rm->Register("\0", 100, "buf");
	SORE_Resource::res_handle buf2 = rm->Register("\0", 100, "buf");
	SORE_Resource::res_handle buf3 = rm->Register("\0", 100, "buf");
	SORE_Resource::res_handle buf4 = rm->Register("\0", 100, "buf");
	SORE_Resource::res_handle buf5 = rm->Register("\0", 100, "buf");
	SORE_Resource::Resource* r;
	SORE_Resource::ResourceData* rd;
	char* data;
	clock_t end,begin,total = 0;
	
	for(int i=0;i<10;i++)
	{
		begin = clock();
		for(int i=0;i<1000000;i++)
		{
			/*r = rm->GetPtr(buf1);
			data = dynamic_cast<SORE_Resource::ResourceData*>(r)->GetDataPtr();
			
			r = rm->GetPtr(buf2);
			data = dynamic_cast<SORE_Resource::ResourceData*>(r)->GetDataPtr();
			
			r = rm->GetPtr(buf3);
			data = dynamic_cast<SORE_Resource::ResourceData*>(r)->GetDataPtr();
			
			r = rm->GetPtr(buf4);
			data = dynamic_cast<SORE_Resource::ResourceData*>(r)->GetDataPtr();
			
			r = rm->GetPtr(buf5);
			data = dynamic_cast<SORE_Resource::ResourceData*>(r)->GetDataPtr();*/
			
			rd = rm->GetDataPtr(buf1);
			data = rd->GetDataPtr();
			
			rd = rm->GetDataPtr(buf2);
			data = rd->GetDataPtr();
			
			rd = rm->GetDataPtr(buf3);
			data = rd->GetDataPtr();
			
			rd = rm->GetDataPtr(buf4);
			data = rd->GetDataPtr();
			
			rd = rm->GetDataPtr(buf5);
			data = rd->GetDataPtr();
		}
		end = clock();
		
		std::cout << "ticks   elapsed = " << end - begin << "\n";
		std::cout << "seconds elapsed = " << double(end-begin)/CLOCKS_PER_SEC << "\n";
		total += end - begin;
	}
	std::cout << "total ticks   elapsed = " << total << "\n";
	std::cout << "total seconds elapsed = " << double(total)/CLOCKS_PER_SEC << "\n";
	std::cout << "avg.  seconds elapsed = " << double(total)/CLOCKS_PER_SEC/10.0 << "\n";
	
	std::cout << "Cleaning up...\n";
	//delete dynamic_cast<SORE_Resource::Buffer*>(r);
	rm->Cleanup();
	return 0;
}
