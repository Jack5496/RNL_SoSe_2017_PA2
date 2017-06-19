#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RNL_PA2");

int main(int argc, char *argv[])
{
  bool verbose = false;
  bool tracing = false;
  double d_udp_bw = 0.0;
  bool tcp_off = false;
  bool westwoodplus = false;
  bool tcp_type_valid = false;
  bool udp_bw_valid = false;
  bool queue_valid = false;

  std::string tcp_type = "Rfc793";
  std::string tcp_type_actual = "TcpRfc793";
  std::string udp_bw = "0Mbps";
  std::string queue = "RED";

  CommandLine cmd;
  cmd.AddValue("tcp_type","Set TCP type", tcp_type);
  cmd.AddValue("udp_bw","UDP Bandwidth", udp_bw);
  cmd.AddValue("queue","Set Gateway queue type", queue);
  cmd.AddValue("verbose","Tell echo application to log", verbose);
  cmd.AddValue("tracing","Enable pcap tracing", tracing);
  cmd.Parse(argc,argv);

  if(tcp_type == "TcpOff"){
    tcp_off = true;
    tcp_type_valid = true;
  }
  if(tcp_type == "Rfc793"){
    tcp_type_actual = "TcpRfc793";
    tcp_type_valid = true;
  }
  if(tcp_type == "TcpTahoe"){
    tcp_type_actual = "TcpTahoe";
    tcp_type_valid = true;
  }
  if(tcp_type == "TcpReno"){
    tcp_type_actual = "TcpReno";
    tcp_type_valid = true;
  }
  if(tcp_type == "TcpNewReno"){
    tcp_type_actual = "TcpNewReno";
    tcp_type_valid = true;
  }
  if(tcp_type == "TcpWestwood"){
    tcp_type_actual = "TcpWestwood";
    tcp_type_valid = true;
  }
  if(tcp_type == "TcpWestwoodPlus"){
    tcp_type_actual = "TcpWestwood";
    westwoodplus = true;
    tcp_type_valid = true;
  }

  if(tcp_type_valid == false){
    NS_ABORT_MSG("Invalid TCP Type.");
  }

  if(udp_bw == "0Mbps"){
    d_udp_bw = 0.0;
    udp_bw_valid = true;
  }
  if(udp_bw == "1Mbps"){
    d_udp_bw = 1.0;
    udp_bw_valid = true;
  }
  if(udp_bw == "2Mbps"){
    d_udp_bw = 2.0;
    udp_bw_valid = true;
  }

  if(udp_bw_valid == false){
    NS_ABORT_MSG("Invalid UDP Type.");
  }
  if(verbose){
    LogComponentEnable("OnOffApplication",LOG_LEVEL_INFO);
    LogComponentEnable("UdpClient",LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer",LOG_LEVEL_INFO);
  }

  if(tcp_off==false){
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",StringValue("ns3::"+tcp_type_actual));
    if(westwoodplus==true){
      Config::SetDefault("ns3::TcpWestwood::ProtocolType",EnumValue(TcpWestwood::WESTWOODPLUS));
    }
  }

  NodeContainer node_0;
  node_0.Create(1);
  NodeContainer node_1;
  node_1.Create(1);
  NodeContainer node_2;
  node_2.Create(1);
  NodeContainer node_3;
  node_3.Create(1);


  PointToPointHelper p2p_0;
  p2p_0.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
  p2p_0.SetChannelAttribute("Delay",StringValue("20ms"));
  PointToPointHelper p2p_1;
  p2p_1.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
  p2p_1.SetChannelAttribute("Delay",StringValue("20ms"));
  PointToPointHelper p2p_2;
  StringValue dr = StringValue("2Mbps");
  StringValue d = StringValue("45ms");
  p2p_2.SetDeviceAttribute("DataRate",dr);
  p2p_2.SetChannelAttribute("Delay",d);


  if(queue=="RED"){
    p2p_2.SetQueue("ns3::RedQueue");
    if(verbose){
      LogComponentEnable("RedQueue",LOG_LEVEL_LOGIC);
    }
    queue_valid = true;
  }
  if(queue=="DropTail"){
    p2p_2.SetQueue("ns3::DropTailQueue");
    if(verbose){
      LogComponentEnable("DropTailQueue",LOG_LEVEL_LOGIC);
    }
    queue_valid = true;
  }

  if(queue_valid==false){
    NS_ABORT_MSG("Invalid queue Type.");
  }


  NodeContainer nodes_p2p_0;
  nodes_p2p_0.Add(node_0);
  nodes_p2p_0.Add(node_2);
  NetDeviceContainer ndc_p2p_0 = p2p_0.Install(nodes_p2p_0);

  NodeContainer nodes_p2p_1;
  nodes_p2p_1.Add(node_1);
  nodes_p2p_1.Add(node_2);
  NetDeviceContainer ndc_p2p_1 = p2p_1.Install(nodes_p2p_1);
  
  NodeContainer nodes_p2p_2;
  nodes_p2p_2.Add(node_2);
  nodes_p2p_2.Add(node_3);
  NetDeviceContainer ndc_p2p_2 = p2p_2.Install(nodes_p2p_2);


  InternetStackHelper internetStackH;
  internetStackH.Install(node_0);
  internetStackH.Install(node_1);
  internetStackH.Install(node_2);
  internetStackH.Install(node_3);


  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.0.0.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_p2p_0 = ipv4.Assign(ndc_p2p_0);
  ipv4.SetBase("10.0.1.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_p2p_1 = ipv4.Assign(ndc_p2p_1);
  ipv4.SetBase("10.0.3.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_p2p_2 = ipv4.Assign(ndc_p2p_2);
  

  if(tcp_off==false){
    uint16_t port_tcp_0 = 6666;
    Address sinkLocalAddress_tcp0 (InetSocketAddress(Ipv4Address::GetAny(),port_tcp_0));
    PacketSinkHelper sinkHelper_tcp_0("ns3::TcpSocketFactory",sinkLocalAddress_tcp0);
    ApplicationContainer sinkApp_tcp_0 = sinkHelper_tcp_0.Install(node_3);
    sinkApp_tcp_0.Start(Seconds(2.0));
    sinkApp_tcp_0.Stop(Seconds(32.0));
    
    OnOffHelper clientHelper_tcp_0("ns3::TcpSocketFactory",iface_ndc_p2p_2.GetAddress(1));
    clientHelper_tcp_0.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    clientHelper_tcp_0.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    clientHelper_tcp_0.SetAttribute("DataRate",StringValue("10Mbps"));
    clientHelper_tcp_0.SetAttribute("PacketSize",UintegerValue(1024));

    ApplicationContainer clientApps_tcp_0;
    AddressValue remoteAddress_tcp_0 (InetSocketAddress (iface_ndc_p2p_2.GetAddress(1),port_tcp_0));
    clientHelper_tcp_0.SetAttribute("Remote",remoteAddress_tcp_0);

    clientApps_tcp_0.Add(clientHelper_tcp_0.Install(node_0));

    clientApps_tcp_0.Start(Seconds(2.0));
    clientApps_tcp_0.Stop(Seconds(32.0));
  }

  uint16_t port_udp = 5555;
  UdpServerHelper server_udp (port_udp);
  ApplicationContainer server_app = server_udp.Install(node_3.Get(0));
  server_app.Start(Seconds(0.9));
  server_app.Stop(Seconds(32.5));

  double udp_start = 1.0;
  double udp_end = 32.0;
  double udp_diff = udp_end-udp_start;
  UdpClientHelper client_udp(iface_ndc_p2p_2.GetAddress(1),port_udp);

  client_udp.SetAttribute("MaxPackets",UintegerValue(d_udp_bw*udp_diff*128));
  client_udp.SetAttribute("Interval",TimeValue(Seconds(1.0/(128.0*d_udp_bw))));
  client_udp.SetAttribute("PacketSize",UintegerValue(1024));
  ApplicationContainer client_app = client_udp.Install(node_1.Get(0));
  client_app.Start(Seconds(udp_start));
  client_app.Stop(Seconds(udp_end));


  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  if(tracing==true){
    std::string pcap ="bench_";
    pcap += tcp_type;
    pcap += "_";
    pcap += queue;
    pcap += "_U";
    pcap += udp_bw;

    p2p_2.EnablePcap(pcap,node_3.Get(0)->GetId(),false,true);
  }

  Simulator::Stop(Seconds(32));

  Simulator::Run();
  Simulator::Destroy();

  return 0;

  
  


}
