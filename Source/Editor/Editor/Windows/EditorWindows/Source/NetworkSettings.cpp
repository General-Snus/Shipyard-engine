#include "../NetworkSettings.h"

#include "imgui.h"
#include "ImGuiHelpers.hpp"
#include <format>
#include <Engine/PersistentSystems/Networking/NetworkRunner.h>
#include <WinSock2.h>


NetworkSettings::NetworkSettings() {}

#pragma optimize( "", off ) 

void NetworkSettings::RenderImGUi() {
	ImGui::Begin(std::format("Network settings##{}",uniqueID).c_str(),&m_KeepWindow);
	ImGui::Markdown(R"(
# Network settings
Here you can adjust network settings to your liking or something.
)");
	static SessionConfiguration cfg;
	static int ip[4] = {127,0,0,1};
	static int port = 27015;
	static int selectedSetting = 0;

	//constexpr auto selection = magic_enum::enum_names<SessionConfiguration::GameMode>();
	static int selected_gameMode = 1;
	ImGui::ComboAutoSelect<SessionConfiguration::GameMode>("Game mode",selected_gameMode,ImGuiComboFlags_HeightSmall); // Infinite love to this, 
	cfg.gameMode = static_cast<SessionConfiguration::GameMode>(selected_gameMode);

	static int selected_hostMode = 1;
	ImGui::ComboAutoSelect<SessionConfiguration::HostType>("Host type",selected_hostMode,ImGuiComboFlags_HeightSmall);
	cfg.hostType = static_cast<SessionConfiguration::HostType>(selected_gameMode);


	//ImGui::ComboAutoSelect("Connection type",&selectedSetting,"Start as server\0Start as client\0");
	ImGui::Separator();
	//Server info

	switch((SessionConfiguration::GameMode)selected_gameMode) {
	case SessionConfiguration::GameMode::Single:
		break;
	case SessionConfiguration::GameMode::Shared:
	case SessionConfiguration::GameMode::Server:
	case SessionConfiguration::GameMode::Host:
	{

		int local[4] = {127,0,0,1};
		ImGui::InputInt4("Ip address",local,ImGuiInputTextFlags_ReadOnly);

		ImGui::InputInt("Port",&port);
		port = std::clamp(port,0,65535);
		cfg.address = NetAddress(std::format("{}.{}.{}.{}",local[0],local[1],local[2],local[3]),(unsigned short)port);
	}
	break;


	case SessionConfiguration::GameMode::Client:
	case SessionConfiguration::GameMode::AutoHostOrClient:
		ImGui::InputInt4("Ip address",ip);
		for(auto& part : ip) {
			part = std::clamp(part,0,255);
		}

		ImGui::InputInt("Port",&port);
		port = std::clamp(port,0,65535);

		cfg.address = NetAddress(std::format("{}.{}.{}.{}",ip[0],ip[1],ip[2],ip[3]),(unsigned short)port);

		break;
	}
	if(ImGui::Button("Attempt connection")) {
		Runner.StartSession(cfg);
	}

	ImGui::SameLine();
	if(ImGui::Button("Close Connection")) {
		Runner.Close();
	}

	ImGui::Separator();
	ImGui::Text(std::format("Server status: {}",magic_enum::enum_name(Runner.connection.GetStatus())).c_str());
	ImGui::Separator();

	if(Runner.IsServer) {
		ImGui::Text("-Remotes\n");
		for(size_t i = 0; i < Runner.remoteConnections.size(); i++) {
			const auto& client = Runner.remoteConnections[i];
			if(!client.isConnected) {
				continue;
			}

			auto timeSince = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::high_resolution_clock::now() - client.lastRecievedMessageTime));
			ImGui::Text(std::format(
				R"(
			Client[{}]:
				Nickname: {} 
				UUID: {} 
				IP: {} 
				Port: {} 
				Ping: {} ms
				Has established udp connection: {}
			)"
				,i,
				client.nickname,
				client.id.id.String(),
				client.remoteConnection.Address().IPStr().c_str(),
				client.remoteConnection.Address().port,
				client.rtt() ,
				client.hasConnectedOverUDP
			).c_str());
		}
	} else
	{
		ImGui::Text(std::format(
			R"(
			My Status:
				Nickname: {} 
				UUID: {} 
				IP: {} 
				TCPPort: {} 
				Has established udp connection: {}
				UDPPort: {} 
				Ping: {} ms
			)",
			Runner.runnerName,
			Runner.runnerID.id.String(),
			Runner.connection.Address().IPStr(),
			Runner.connection.Address().port,
			Runner.udpConnectionInitialized,
			Runner.serverUdpConnection.port,
			Runner.heartBeatSystem.rtt()
		).c_str());


	}

	ImGui::End();
}
#pragma optimize( "", on )

