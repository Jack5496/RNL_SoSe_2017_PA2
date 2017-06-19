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
  /* * * * * * * * *  */
  /* Config Variablen */
  /* * * * * * * * *  */

  int error_level = 0; /* hoffentlich nie > 0*/
  double datenrate_udp_bw = 0.0;
  int maxPacketSize = 1024;
  double kilobyte = 1024.0;
  double byte = 8.0;

  bool wwp = false; /* TCP WestwoodPlus verwendet? */

  double app_start = 5.0; /* Falls NS3 Anlaufprobleme hat? */
  double app_duration = 30.0;

  
  /* Minirätsel: Warum habe ich die Portnummern so vergeben (^^)_(;,,;)_(^^) */
  uint16_t port_TCP = 827;
  uint16_t port_UDP = 837;

/* Links zum Gateway */
  std::string p2p_sender_dr_in_mbps = "10";  /* Datenrate in Mbp/s */
  std::string p2p_sender_delay_in_ms = "20"; /* Delay in ms */

  /* Link vom Gateway zur Senke */
  std::string p2p_gateway_dr_in_mbps = "2"; /* Datenrate in Mbp/s */
  std::string p2p_gateway_delay_in_ms = "45"; /* Delay in ms */
  
  /* Variablen für Valide Parameter */
  bool tcp_type_valid = false;
  bool udp_bw_valid = false;
  bool queue_valid = false;  

  std::string tcp_type = "Rfc793";
  std::string tcp_type_actual = "TcpRfc793";
  std::string queue = "RED"; /* Standartmäßig RED */
  std::string udp_bw = "0Mbps"; /* Standartmäßig aus */

   bool log_level_enabled = false;

  bool pcap_tracing = false;
  CommandLine cmd;

  /* TCP Typ welcher getestet werden soll */
  cmd.AddValue("tcp_type","Setze den TCP Typ", tcp_type);

  /* Verwendende Queue Implementation */
  cmd.AddValue("queue","Set Queue Typ", queue);
  
  /* Zu testende UDP Geschwindigkeit */
  cmd.AddValue("udp_bw","Setze die UDP Bandbreite", udp_bw);
    
  /* Anzeigen von Detailierteren/Debug Information */
  cmd.AddValue("log_level_enabled","Zeige detailiertere Informationen", log_level_enabled);

  cmd.AddValue("pcap_tracing","Erstelle ein Pcap trace File", pcap_tracing);

  /* Parse alle Parameter in die Variablen */
  cmd.Parse(argc,argv);

  
  /* Hier bekomme ich einen Segfault

  std::string valid_typ_types[] = {"Rfc793","TcpRfc793","TcpTahoe","TcpReno","TcpNewReno","TcpWestwood","TcpWestwoodPlus"};
  // Durchlaufe alle validen TCP Typen 
  for(unsigned int pos=0; pos < sizeof(valid_typ_types); pos=pos+1){
    // Falls es ein valider Typ ist 
    std::string cmp = valid_typ_types[pos];
    if(tcp_type == cmp){
      tcp_type_actual = cmp; // merke welcher Typ es war 
      tcp_type_valid = true; // merke dass es ein valider Typ war 
    }    
  }
  */

  if(tcp_type == "Rfc793" || tcp_type == "TcpTahoe" || tcp_type == "TcpReno" || tcp_type == "TcpNewReno" || tcp_type == "TcpWestwood" || tcp_type == "TcpWestwoodPlus"){
    tcp_type_actual = tcp_type;
    tcp_type_valid = true;
  }

  /* Dank der tollen Sonderfälle überprüfen wir diese*/
  if(tcp_type =="Rfc793") {
    /* Rfc793 hat in NS-3 eine besondere Bezeichnung */
    tcp_type_actual = "TcpRfc793";
  }
  if(tcp_type == "TcpWestwoodPlus"){
    /* WestwoodPlus wie WestwoodPlus erstellt werden */
    tcp_type_actual = "TcpWestwood";
    wwp = true; /* und nacher angepasst werden */
    
  }

  /* Ausstieg falls Fehlerhafter TCP Typ */
  if(tcp_type_valid == false){
    NS_ABORT_MSG("Der TCP Typ war fehlerhaft.");    
    error_level=error_level+1;
    return error_level; /* Wir sind raus ! */
  }

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
  /* Überprüfen der Übergebenen Parameter für UDP Geschwindigkeit */
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
  if(udp_bw == "0Mbps" || udp_bw == "1Mbps" || udp_bw == "2Mbps"){
    datenrate_udp_bw = 1.0*atoi(udp_bw.c_str());
    udp_bw_valid = true;
  }

  /* Ausstieg falls fehlerhafte UDP Datenrate */  
  if(udp_bw_valid == false){
    NS_ABORT_MSG("Die UDP Bandbreite war fehlerhaft");
    error_level=error_level+1;
    return error_level; /* Wir sind raus ! */
  }
  /* Setzte die Log Componenten falls gewünscht */
  if(log_level_enabled){
    /* Setzte auf INFO LOG_LEVEL */
    LogComponentEnable("OnOffApplication",LOG_LEVEL_INFO);
    LogComponentEnable("UdpClient",LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer",LOG_LEVEL_INFO);
  }

  /*  * * * * * * * * * * */
  /* TCP Protokoll setzen */
  /*  * * * * * * * * * * */
  Config::SetDefault("ns3::TcpL4Protocol::SocketType",StringValue("ns3::"+tcp_type_actual));
  /* Westwoodplus Spezialfall */
  if(wwp==true){
    Config::SetDefault("ns3::TcpWestwood::ProtocolType",EnumValue(TcpWestwood::WESTWOODPLUS));
  }

  /* * * * * * * * * *  */
  /* Netzwerk Topologie */
  /* * * * * * * * * *  */

/* Erstelle Nodes */
  /* Am schönsten wären hier Array, leider Segfaults */
  NodeContainer node_tcp,node_udp,node_gate,node_rec;
  node_tcp.Create(1); /* TCP Sender */
  node_udp.Create(1); /* UDP Sender */
  node_gate.Create(1); /* Gateway */
  node_rec.Create(1); /* Reciever */

/* Erstelle Links */
  PointToPointHelper p2p_tcp,p2p_udp,p2p_gate;
  p2p_tcp.SetDeviceAttribute("DataRate",StringValue(p2p_sender_dr_in_mbps+"Mbps"));
  p2p_tcp.SetChannelAttribute("Delay",StringValue(p2p_sender_delay_in_ms+"ms"));
  
  p2p_udp.SetDeviceAttribute("DataRate",StringValue(p2p_sender_dr_in_mbps+"Mbps"));
  p2p_udp.SetChannelAttribute("Delay",StringValue(p2p_sender_delay_in_ms+"ms"));
  
  /* Der Link des Gateways unterscheidet sich */
  p2p_gate.SetDeviceAttribute("DataRate",StringValue(p2p_gateway_dr_in_mbps+"Mbps"));
  p2p_gate.SetChannelAttribute("Delay",StringValue(p2p_gateway_delay_in_ms+"ms"));

/* Identifierziere Queue Typ */
 std::string queue_type = "";

  if(queue=="RED"){
    queue_type = "RedQueue";
    queue_valid = true;
  }
  if(queue=="DropTail"){
    queue_type = "DropTailQueue";
    queue_valid = true;
  }

/* Überprüfe ob Valider Queue Typ */
  if(queue_valid==false){
    NS_ABORT_MSG("Invalid queue Type.");
  }

  /* Setze den Queue Typ, da valide */
  p2p_gate.SetQueue("ns3::"+queue_type);
  if(log_level_enabled){
    /* Schalte ggf. Log ein */
    LogComponentEnable(queue_type.c_str(),LOG_LEVEL_LOGIC);
  }


  /* Erstelle NodeContainers */
  /* Am schönsten wären hier Array, leider Segfaults */
  NodeContainer nodes_tcp2gate,nodes_udp2gate,nodes_gate2rec;
  
  nodes_tcp2gate.Add(node_tcp);
  nodes_tcp2gate.Add(node_gate);
  
  nodes_udp2gate.Add(node_udp);
  nodes_udp2gate.Add(node_gate);
  
  nodes_gate2rec.Add(node_gate);
  nodes_gate2rec.Add(node_rec);
  
  /* Erstelle NetDeviceContainer */
  NetDeviceContainer ndc_tcp2gate = p2p_tcp.Install(nodes_tcp2gate);
  NetDeviceContainer ndc_udp2gate = p2p_udp.Install(nodes_udp2gate);
  NetDeviceContainer ndc_gate2rec = p2p_gate.Install(nodes_gate2rec);

  /* Installiere alle Nodes */
  InternetStackHelper internetStackH;
  internetStackH.Install(node_tcp);
  internetStackH.Install(node_udp);
  internetStackH.Install(node_gate);
  internetStackH.Install(node_rec);

  /* Setze alle IP Addressen der Nodes */
  /* Am schönsten wären hier Loop, Segfault */
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.0.0.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_tcp2gate = ipv4.Assign(ndc_tcp2gate);
  ipv4.SetBase("10.0.1.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_udp2gate = ipv4.Assign(ndc_udp2gate);
  ipv4.SetBase("10.0.3.0","255.255.255.0");
  Ipv4InterfaceContainer iface_ndc_gate2rec = ipv4.Assign(ndc_gate2rec);
  
  /* Berechne Zeitpunkt des Stops */
  double app_end = app_duration+app_start;

  /* Lege Addresse des Empfängers fest */
  Address address_rec (InetSocketAddress(Ipv4Address::GetAny(),port_TCP));
  PacketSinkHelper packetSH_rec("ns3::TcpSocketFactory",address_rec);
  ApplicationContainer appC_rec = packetSH_rec.Install(node_rec);
  /* Bestimmte Zeit der Aufnahme */
  appC_rec.Start(Seconds(app_start));
  appC_rec.Stop(Seconds(app_end));
    
  OnOffHelper onofhelper("ns3::TcpSocketFactory",iface_ndc_gate2rec.GetAddress(1));
  ApplicationContainer appcontainer;
  AddressValue remoteAddress_tcp_0 (InetSocketAddress (iface_ndc_gate2rec.GetAddress(1),port_TCP));

  onofhelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onofhelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  onofhelper.SetAttribute("DataRate",StringValue(p2p_sender_dr_in_mbps+"Mbps"));
  onofhelper.SetAttribute("PacketSize",UintegerValue(maxPacketSize));
  onofhelper.SetAttribute("Remote",remoteAddress_tcp_0);
  
  appcontainer.Add(onofhelper.Install(node_tcp));
  appcontainer.Start(Seconds(app_start));
  appcontainer.Stop(Seconds(app_end));

  UdpServerHelper server_udp (port_UDP);
  ApplicationContainer server_app = server_udp.Install(node_rec.Get(0));
  server_app.Start(Seconds(app_start));
  server_app.Stop(Seconds(app_end));

  
  double simulationsdauer = app_end-app_start;
  UdpClientHelper client_udp(iface_ndc_gate2rec.GetAddress(1),port_UDP);

  client_udp.SetAttribute("MaxPackets",UintegerValue(datenrate_udp_bw*simulationsdauer*(kilobyte/byte)));
  client_udp.SetAttribute("Interval",TimeValue(Seconds(1.0/((kilobyte/byte)*datenrate_udp_bw))));
  client_udp.SetAttribute("PacketSize",UintegerValue(maxPacketSize));
  ApplicationContainer client_app = client_udp.Install(node_udp.Get(0));
  client_app.Start(Seconds(app_start));
  client_app.Stop(Seconds(app_end));


  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  if(pcap_tracing==true){
    std::string pcap ="bench_"+tcp_type+"_"+queue+"_U"+udp_bw;
    p2p_gate.EnablePcap(pcap,node_rec.Get(0)->GetId(),false,true);
  }

  Simulator::Stop(Seconds(app_end));

  Simulator::Run();
  Simulator::Destroy();

  return 0;

  
  


}