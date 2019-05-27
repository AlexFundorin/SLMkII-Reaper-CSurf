/*
** reaper_csurf
** Copyright (C) 2006-2008 Cockos Incorporated
** License: LGPL.
*/

#define REAPERAPI_IMPLEMENT
#define REAPERAPI_DECL

#ifndef _CSURF_H_
#include "csurf.h"
#endif

#include "routines/sl_global.h"

extern reaper_csurf_reg_t csurf_sl_reg;

REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs, if any
HWND g_hwnd;

 //Added by fundorin (SWS extra functions)
double(*BR_GetPrevGridDivision)(double position) = NULL;
double(*BR_GetNextGridDivision)(double position) = NULL;


// Juce GUI related classes and functions
reaper_plugin_info_t* g_plugin_info;

class action_entry
{ //class for registering actions
public:
	action_entry(std::string description, std::string idstring, toggle_state togst, std::function<void(action_entry&)> func);
	action_entry(const action_entry&) = delete; // prevent copying
	action_entry& operator=(const action_entry&) = delete; // prevent copying
	action_entry(action_entry&&) = delete; // prevent moving
	action_entry& operator=(action_entry&&) = delete; // prevent moving

	int m_command_id = 0;
	gaccel_register_t m_accel_reg;
	std::function<void(action_entry&)> m_func;
	std::string m_desc;
	std::string m_id_string;
	int m_val = 0;
	int m_valhw = 0;
	int m_relmode = 0;
	toggle_state m_togglestate = CannotToggle;
	void* m_data = nullptr;
	template<typename T>
	T* getDataAs() { return static_cast<T*>(m_data); }
};

action_entry::action_entry(std::string description, std::string idstring, toggle_state togst, std::function<void(action_entry&)> func) :
	m_desc(description), m_id_string(idstring), m_func(func), m_togglestate(togst)
{

	if (g_plugin_info != nullptr)
	{
		m_accel_reg.accel = { 0,0,0 };
		m_accel_reg.desc = m_desc.c_str();
		m_accel_reg.accel.cmd = m_command_id = g_plugin_info->Register("command_id", (void*)m_id_string.c_str());
		g_plugin_info->Register("gaccel", &m_accel_reg);
	}
}

std::vector<std::shared_ptr<action_entry>> g_actions;

std::shared_ptr<action_entry> add_action(std::string name, std::string id, toggle_state togst,
	std::function<void(action_entry&)> f)
{
	auto entry = std::make_shared<action_entry>(name, id, togst, f);
	g_actions.push_back(entry);
	return entry;
}

// Reaper calls back to this when it wants to know an actions's toggle state
int toggleActionCallback(int command_id)
{
	for (auto& e : g_actions)
	{
		if (command_id != 0 && e->m_togglestate != CannotToggle && e->m_command_id == command_id)
		{
			if (e->m_togglestate == ToggleOff)
				return 0;
			if (e->m_togglestate == ToggleOn)
				return 1;
		}
	}
	// -1 if action not provided by this extension or is not togglable
	return -1;
}


extern "C"
{
	REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *reaper_plugin_info)
	{
		g_hInst = hInstance;
		g_plugin_info = reaper_plugin_info;
		//g_hwnd = reaper_plugin_info->hwnd_main;

		if (!reaper_plugin_info || reaper_plugin_info->caller_version != REAPER_PLUGIN_VERSION || !reaper_plugin_info->GetFunc)
			return 0;

		// Adding new actions to Reaper's action list
		add_action("SL JUCE test : Show/hide CSurf Logger", "JUCETEST_SHOW_CSURFLOGGER", ToggleOff, [](action_entry& ae)
			{
				// add function
			});

		add_action("SL JUCE test : Test user inputs", "JUCETEST_USERINPUTSEX", ToggleOff, [](action_entry& ae)
			{
				// add function
			});

		//add_action("SL JUCE test : MIDI/OSC action test", "JUCETEST_MIDIOSCTEST", CannotToggle, onActionWithValue);

		//reaper_plugin_info->Register("hookcommand2", (void*)on_value_action);
		reaper_plugin_info->Register("toggleaction", (void*)toggleActionCallback);
		//MySurface* surf = new MySurface;


		if (reaper_plugin_info)
		{

			g_hwnd = reaper_plugin_info->hwnd_main;

			if (REAPERAPI_LoadAPI(reaper_plugin_info->GetFunc)) return 0;

			reaper_plugin_info->Register("csurf", &csurf_sl_reg);

			// plugin registered
			return 1;
		}

		else
		{
			return 0;
		}
	}
};





#ifndef _WIN32 // MAC resources
#include "../Library/WDL/swell/swell-dlggen.h"
#include "res.rc_mac_dlg"
#undef BEGIN
#undef END
#include "../Library/WDL/swell/swell-menugen.h"
#include "res.rc_mac_menu"
#endif


#ifndef _WIN32 // let OS X use this threading step

#include "../Library/WDL/mutex.h"
#include "../Library/WDL/ptrlist.h"



class threadedMIDIOutput : public midi_Output
{
public:
	threadedMIDIOutput(midi_Output *out)
	{
		m_output = out;
		m_quit = false;
		DWORD id;
		m_hThread = CreateThread(NULL, 0, threadProc, this, 0, &id);
	}
	virtual ~threadedMIDIOutput()
	{
		if (m_hThread)
		{
			m_quit = true;
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = 0;
			Sleep(30);
		}

		delete m_output;
		m_empty.Empty(true);
		m_full.Empty(true);
	}

	virtual void SendMsg(MIDI_event_t *msg, int frame_offset) // frame_offset can be <0 for "instant" if supported
	{
		if (!msg) return;

		WDL_HeapBuf *b = NULL;
		if (m_empty.GetSize())
		{
			m_mutex.Enter();
			b = m_empty.Get(m_empty.GetSize() - 1);
			m_empty.Delete(m_empty.GetSize() - 1);
			m_mutex.Leave();
		}
		if (!b && m_empty.GetSize() + m_full.GetSize() < 500)
			b = new WDL_HeapBuf(256);

		if (b)
		{
			int sz = msg->size;
			if (sz < 3)sz = 3;
			int len = msg->midi_message + sz - (unsigned char *)msg;
			memcpy(b->Resize(len, false), msg, len);
			m_mutex.Enter();
			m_full.Add(b);
			m_mutex.Leave();
		}
	}

	virtual void Send(unsigned char status, unsigned char d1, unsigned char d2, int frame_offset) // frame_offset can be <0 for "instant" if supported
	{
		MIDI_event_t evt = { 0,3,status,d1,d2 };
		SendMsg(&evt, frame_offset);
	}

	///////////

	static DWORD WINAPI threadProc(LPVOID p)
	{
		WDL_HeapBuf *lastbuf = NULL;
		threadedMIDIOutput *_this = (threadedMIDIOutput*)p;
		unsigned int scnt = 0;
		for (;;)
		{
			if (_this->m_full.GetSize() || lastbuf)
			{
				_this->m_mutex.Enter();
				if (lastbuf) _this->m_empty.Add(lastbuf);
				lastbuf = _this->m_full.Get(0);
				_this->m_full.Delete(0);
				_this->m_mutex.Leave();

				if (lastbuf) _this->m_output->SendMsg((MIDI_event_t*)lastbuf->Get(), -1);
				scnt = 0;
			}
			else
			{
				Sleep(1);
				if (_this->m_quit&&scnt++ > 3) break; //only quit once all messages have been sent
			}
		}
		delete lastbuf;
		return 0;
	}

	WDL_Mutex m_mutex;
	WDL_PtrList<WDL_HeapBuf> m_full, m_empty;

	HANDLE m_hThread;
	bool m_quit;
	midi_Output *m_output;
};




midi_Output *CreateThreadedMIDIOutput(midi_Output *output)
{
	if (!output) return output;
	return new threadedMIDIOutput(output);
}

#else

// windows doesnt need it since we have threaded midi outputs now
midi_Output *CreateThreadedMIDIOutput(midi_Output *output)
{
	return output;
}

#endif