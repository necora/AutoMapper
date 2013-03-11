#include "P4Command.h"
#include "P4Task.h"
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

class P4ConfigCommand : public P4Command
{
public:
	P4ConfigCommand(const char* name) : P4Command(name) {}
	virtual bool Run(P4Task& task, const CommandArgs& args)
	{
		if (args.size() < 2)
		{
			string msg = "Perforce plugin got invalid config setting :"; 
			for (CommandArgs::const_iterator i = args.begin(); i != args.end(); ++i) {
				msg += " ";
				msg += *i;
			}
			Pipe().WarnLine(msg, MAConfig);
			Pipe().EndResponse();
			return true;
		}
		
		string key = args[1];
		string value = args.size() > 2 ? args[2] : string();
		
		ClearStatus();
		
		string logValue = value;
		if (key == "vcPerforcePassword")
			logValue = "*";
		Pipe().Log() << "Got config " << key << " = '" << logValue << "'" << unityplugin::Endl;

		// This command actually handles several commands all 
		// concerning connecting to the perforce server
		if (key == "vcPerforceUsername")
		{
			task.SetP4User(value);
		}
		else if (key == "vcPerforceWorkspace")
		{
			task.SetP4Client(value);
		}
		else if (key == "assetsPath")
		{
			task.SetAssetsPath(value);
		}
		else if (key == "vcPerforcePassword")
		{
			task.SetP4Password(value);
			value = "*";
		}
		else if (key == "vcPerforceServer")
		{
			string::size_type i = value.find(":");
			if (i == string::npos)
				value += ":1666"; // default port
			task.SetP4Port(value);
		}
		else if (key == "pluginVersions")
		{
			int sel = SelectVersion(args);
			Pipe().OkLine(sel, MAConfig); 
			Pipe().Log() << "Selected plugin protocol version " << sel << unityplugin::Endl;
		}
		else if (key == "pluginTraits")
		{
			Pipe().OkLine("4");
			Pipe().OkLine("requiresNetwork", MAConfig); 			
			Pipe().OkLine("enablesCheckout", MAConfig);
			Pipe().OkLine("enablesLocking", MAConfig);
			Pipe().OkLine("enablesRevertUnchanged", MAConfig);
		
			Pipe().OkLine("4");
			Pipe().OkLine("vcPerforceUsername");
			Pipe().OkLine("Username", MAConfig);
			Pipe().OkLine("The perforce user name", MAConfig);
			Pipe().OkLine("");
			Pipe().OkLine("1"); // required field

			Pipe().OkLine("vcPerforcePassword");
			Pipe().OkLine("Password", MAConfig);
			Pipe().OkLine("The perforce password", MAConfig);
			Pipe().OkLine("");
			Pipe().OkLine("3"); // required field | password field

			Pipe().OkLine("vcPerforceWorkspace");
			Pipe().OkLine("Workspace", MAConfig);
			Pipe().OkLine("The perforce workspace/client", MAConfig);
			Pipe().OkLine("");
			Pipe().OkLine("1"); // required field

			Pipe().OkLine("vcPerforceServer");
			Pipe().OkLine("Server", MAConfig);
			Pipe().OkLine("The perforce server using format: hostname:port. Port hostname defaults to 'perforce' and port defaults to 1666", MAConfig);
			Pipe().OkLine("perforce");
			Pipe().OkLine("1"); // required field
		} 
		else 
		{
			Pipe().WarnLine(ToString("Unknown config field set on version control plugin: ", key), MAConfig);
		}
		Pipe().EndResponse();
		return true;
	}
	
	int SelectVersion(const CommandArgs& args)
	{
		set<int> unitySupportedVersions;
		set<int> pluginSupportedVersions;
		
		pluginSupportedVersions.insert(1);
		
		// Read supported versions from unity
		CommandArgs::const_iterator i = args.begin();
		i += 2;
		for	( ; i != args.end(); ++i)
		{
			unitySupportedVersions.insert(atoi(i->c_str()));
		}
		
		set<int> candidates;
		set_intersection(unitySupportedVersions.begin(), unitySupportedVersions.end(),
						 pluginSupportedVersions.begin(), pluginSupportedVersions.end(),
						 inserter(candidates, candidates.end()));
		if (candidates.empty())
			return -1;
		
		return *candidates.rbegin();
	}
	
} cConfig("pluginConfig");

