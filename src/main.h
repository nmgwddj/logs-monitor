#pragma once

#include "resource.h"
#include "shared/ui/msgbox.h"

class MiscThread : public nbase::FrameworkThread
{
public:
	MiscThread(nbase::ThreadId thread_id, const char *name)
		: FrameworkThread(name), thread_id_(thread_id) {}
	~MiscThread(void) {}

private:
	virtual void Init() override
	{
		nbase::ThreadManager::RegisterThread(thread_id_);
	}
	virtual void Cleanup() override
	{
		nbase::ThreadManager::UnregisterThread();
	}

private:
	nbase::ThreadId thread_id_;
};

class MainThread : public nbase::FrameworkThread
{
public:
	MainThread() : nbase::FrameworkThread("MainThread") {}
	virtual ~MainThread() {}

private:
	virtual void Init() override;
	virtual void Cleanup() override;
	void PreMessageLoop();
	void PostMessageLoop();

private:
	std::unique_ptr<MiscThread> misc_thread_;
	std::unique_ptr<MiscThread> db_thread_;
};
