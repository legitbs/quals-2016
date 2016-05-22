#ifndef GLADOS_CLASSES
#define GLADOS_CLASSES

class BaseCore
{
	public:
		typedef enum CoreEnum
		{
			Base = 0,
			Glados,
			CaveJohnson,
			DataArray,
			MemoryInfo,
			RandomNumber,
			BufferOverflow,
			RawData,
			CORE_COUNT
		} CoreEnum;

		BaseCore() {}
		virtual ~BaseCore() {}
		virtual char *MenuString() {return "Base"; }
		virtual CoreEnum CoreType() { return Base; }
		virtual int Action(int FromCaveJohnson) { return 0; }
		virtual int TotalMemSize() { return sizeof(BaseCore); }
		virtual char *Description() {return "Base core used as a framework for all core creation"; }
	private:
};

class DataArrayCore : public BaseCore
{
	public:
		DataArrayCore();
		DataArrayCore(int ArraySize);
		DataArrayCore(int ArraySize, char **List);
		virtual ~DataArrayCore();
		char *MenuString() { return "Data Array"; }
		BaseCore::CoreEnum CoreType() { return DataArray; }
		void SetEntry(unsigned long Index, unsigned long Data);
		unsigned long GetEntry(long Index);
		int GetCount() { return _DataArraySize; }
		int Action(int FromCaveJohnson);
		int TotalMemSize();
		char *Description() {return "A basic core for handling an array of information. Not smart but does get it's job done"; }
	private:
		void InitArray(int FromCaveJohnson);
		unsigned long *_DataArray;
		unsigned long _DataArraySize;
};

class GladosCore : public BaseCore
{
	public:
		GladosCore();
		virtual ~GladosCore();
		char *MenuString() { return "Glados"; }
		BaseCore::CoreEnum CoreType() { return Glados; }
		int Action(int FromCaveJohnson);
		int TotalMemSize();
		char *Description() {return "The GLADOS core, always causing trouble and never in a good mood"; }
	protected:
		DataArrayCore *_GoodMsgs;
		DataArrayCore *_BadMsgs;
};

class CaveJohnsonCore : public GladosCore
{
	public:
		CaveJohnsonCore();
		BaseCore::CoreEnum CoreType() { return CaveJohnson; }
		char *MenuString() { return "Cave Johnson"; }
		int Action(int FromCaveJohnson);
		char *Description() {return "A recreation of the consciousness of Cave Johnson including his method of testing"; } 
};

class RandomNumberCore : public BaseCore
{
	public:
		RandomNumberCore();
		virtual ~RandomNumberCore();
		char *MenuString() { return "Random Number"; }
		BaseCore::CoreEnum CoreType() { return RandomNumber; }
		int Action(int FromCaveJohnson);
		int TotalMemSize();
		char *Description() {return "A core that enjoys creating and giving out random numbers for testing"; }
	private:
		void RefillList();
		DataArrayCore *_RandomNumberList;
		unsigned int _CurEntry;	
};

class BufferOverflowCore : public BaseCore
{
	public:
		BufferOverflowCore();
		virtual ~BufferOverflowCore();
		char *MenuString() { return "Buffer Overflow"; }
		BaseCore::CoreEnum CoreType() { return BufferOverflow; }
		int Action(int FromCaveJohnson);
		char *Description() {return "A simple core that tries to cause trouble. A close eye has to be kept on it"; }
	private:
};

class RawDataCore : public BaseCore
{
	public:
		RawDataCore();
		virtual ~RawDataCore();
		char *MenuString() { return "Raw Data"; }
		BaseCore::CoreEnum CoreType() { return RawData; }
		int Action(int FromCaveJohnson);
		int TotalMemSize();
		char *Description() {return "Enjoys the raw treats of life"; }
	private:
		void InitBuffer(int FromCaveJohnson);
		unsigned int _BufferSize;
		char *_Buffer;
};

class MemoryInfoCore : public BaseCore
{
	public:
		MemoryInfoCore();
		virtual ~MemoryInfoCore();
		char *MenuString() { return "Memory Info"; }
		BaseCore::CoreEnum CoreType() { return MemoryInfo; }
		int Action(int FromCaveJohnson);
		char *Description() {return "Loves information and statistics"; }
};

#endif
