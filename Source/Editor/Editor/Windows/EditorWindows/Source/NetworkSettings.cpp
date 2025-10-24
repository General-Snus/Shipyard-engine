#include "../NetworkSettings.h"

#include "imgui.h"
#include "Editor/Editor/Helpers/ImGuiHelpers.h"
#include <format>
#include <Engine/PersistentSystems/Networking/NetworkRunner.h>
#include <WinSock2.h>
#include "misc\cpp\imgui-combo-filter.h"
#include "Engine\GraphicsEngine\Renderer.h"


NetworkSettings::NetworkSettings() {}

void NetworkSettings::RenderImGUi()
{
	ImGui::Begin(std::format("Network settings##{}", uniqueID).c_str(), &m_KeepWindow);
	ImGui::Markdown(R"(
# Network settings
Here you can adjust network settings to your liking or something.
)");
	static SessionConfiguration cfg;
	static int ip[4] = { 127,0,0,1 };
	static int port = 27015;
	static int selectedSetting = 0;

	//constexpr auto selection = magic_enum::enum_names<SessionConfiguration::GameMode>();
	static int selected_gameMode = 1;
	ImGui::ComboAutoSelect<SessionConfiguration::GameMode>("Game mode", selected_gameMode, ImGuiComboFlags_HeightSmall); // Infinite love to this, 
	cfg.gameMode = static_cast<SessionConfiguration::GameMode>(selected_gameMode);

	static int selected_hostMode = 1;
	ImGui::ComboAutoSelect<SessionConfiguration::HostType>("Host type", selected_hostMode, ImGuiComboFlags_HeightSmall);
	cfg.hostType = static_cast<SessionConfiguration::HostType>(selected_gameMode);


	//ImGui::ComboAutoSelect("Connection type",&selectedSetting,"Start as server\0Start as client\0");
	ImGui::Separator();
	//Server info

	switch ((SessionConfiguration::GameMode)selected_gameMode)
	{
	case SessionConfiguration::GameMode::Single:
		break;
	case SessionConfiguration::GameMode::Shared:
	case SessionConfiguration::GameMode::Server:
	case SessionConfiguration::GameMode::Host:
	{

		int local[4] = { 127,0,0,1 };
		ImGui::InputInt4("Ip address", local, ImGuiInputTextFlags_ReadOnly);

		ImGui::InputInt("Port", &port);
		port = std::clamp(port, 0, 65535);
		cfg.address = NetAddress(std::format("{}.{}.{}.{}", local[0], local[1], local[2], local[3]), (unsigned short)port);
	}
	break;


	case SessionConfiguration::GameMode::Client:
	case SessionConfiguration::GameMode::AutoHostOrClient:
		ImGui::InputInt4("Ip address", ip);
		for (auto& part : ip)
		{
			part = std::clamp(part, 0, 255);
		}

		ImGui::InputInt("Port", &port);
		port = std::clamp(port, 0, 65535);

		cfg.address = NetAddress(std::format("{}.{}.{}.{}", ip[0], ip[1], ip[2], ip[3]), (unsigned short)port);

		break;
	}
	if (ImGui::Button("Attempt connection"))
	{
		Runner.StartSession(cfg);
	}

	ImGui::SameLine();
	if (ImGui::Button("Close Connection"))
	{
		Runner.Close();
	}

	ImGui::Separator();
	ImGui::Text(std::format("Server status: {}", magic_enum::enum_name(Runner.connection.GetStatus())).c_str());
	ImGui::Separator();


	static bool showAOI = false;

	ImGui::ToggleButton("AOITOggle", &showAOI, "Show area of interest");

	///Average out the down&Uplink
	if (Runner.IsServer)
	{
		downlink.Add(Runner.downlinkRate());
		uplink.Add(Runner.uplinkRate());

		ImGui::Text("-Remotes\n");
		for (int i = 0; i < Runner.remoteConnections.size(); i++)
		{
			const auto& client = Runner.remoteConnections[i];
			if (!client.isConnected)
			{
				continue;
			}

			//WARNING ERRORNOUS CODE, only works if only 1 client is connected. IDGAF
			downlink.Add(Runner.downlinkRate(i));
			uplink.Add(Runner.uplinkRate(i));

			auto timeSince = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::high_resolution_clock::now() - client.lastRecievedMessageTime));
			ImGui::Text(std::format(
				R"(
			Client[{}]:
				Nickname: {} 
				UUID: {} 
				IP: {} 
				Port: {}  
				Current Ping: {}  
				Has established udp connection: {}
				Downlink: {}
				Uplink: {}
				Loss: {}
			)"
				, i,
				client.nickname,
				client.id.id.String(),
				client.remoteConnection.Address().IPStr().c_str(),
				client.remoteConnection.Address().port,
				client.rtt(),
				client.hasConnectedOverUDP,
				downlink.Average(),
				uplink.Average(),
				(uplink.Average() / client.dataSent.Average())
			).c_str());

			DrawPingPlot(client.rtt());

			if (showAOI)
			{
				auto sphere = client.areaOfInterest.area;
				GetRenderer().debugDrawer.AddDebugSphere(sphere.Center, sphere.Radius, Colors::red, .01f);
			}
		}
	}
	else
	{
		downlink.Add(Runner.downlinkRate());
		uplink.Add(Runner.uplinkRate());

		ImGui::Text(std::format(
			R"(
			My Status:
				Nickname: {} 
				UUID: {} 
				IP: {} 
				TCPPort: {} 
				UDPPort: {} 
				Current Ping: {} ms
				Has established udp connection: {}
				Downlink: {}
				Uplink: {}
				Loss: {}
			)",
			Runner.runnerName,
			Runner.runnerID.id.String(),
			Runner.connection.Address().IPStr(),
			Runner.connection.Address().port,
			Runner.serverUdpConnection.port,
			Runner.heartBeatSystem.rtt(),
			Runner.udpConnectionInitialized,
			downlink.Average(),
			uplink.Average(),
			uplink.Average() / Runner.heartBeatSystem.downlinkRate()

		).c_str());
		DrawPingPlot(Runner.heartBeatSystem.rtt());

		if (showAOI)
		{
			auto sphere = Runner.layer.AOI().area;
			GetRenderer().debugDrawer.AddDebugSphere(sphere.Center, sphere.Radius, Colors::red, .01f);
		}
	}

	ImGui::End();
}
void NetworkSettings::DrawPingPlot(float newPing)
{
	constexpr auto size = 1000;
	static float rttTime[size] = { 0.0f };
	static int index = -1;
	index++;
	if (index >= size)
	{
		index = 0;
	}
	rttTime[index] = newPing;
	ImGui::PlotLines("Ping", &rttTime[0], size, index);
}
