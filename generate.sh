#!/bin/bash

output="Auswertung/"
path="scratch/"
program="tu_bench_955625"
pcapstart="bench_"
pcapend1=".pcap"
pcapend2="-3-0.pcap"
u1="_"
u2="_U"

tcp_type=("Rfc793" "TcpTahoe" "TcpReno" "TcpNewReno" "TcpWestwood" "TcpWestwoodPlus")
udp_bw=("0Mbps" "1Mbps" "2Mbps")
queue=("RED" "DropTail")

mkdir -p $output

for tcp_type_e in "${tcp_type[@]}"
do
	for queue_e in "${queue[@]}"
	do
		for udp_bw_e in "${udp_bw[@]}"
		do
			echo "start: $tcp_type_e-$udp_bw_e-$queue_e"
			mkdir -p "$output/$tcp_type_e-$queue_e"
			./waf --run "tu_bench_955625 --tcp_type=$tcp_type_e --udp_bw=$udp_bw_e --queue=$queue_e --tracing=true --verbose=false" --cwd $output #"$output$tcp_type_e-$udp_bw_e-$queue_e"
			#echo "Move: $output$pcapstart$tcp_type_e$u1$queue_e$u2$udp_bw_e$pcapend2 --> $output$tcp_type_e-$queue_e/$pcapstart$tcp_type_e$u1$queue_e$u1$udp_bw_e$pcapend1"
			mv "$output$pcapstart$tcp_type_e$u1$queue_e$u2$udp_bw_e$pcapend2" "$output$tcp_type_e-$queue_e/$pcapstart$tcp_type_e$u1$queue_e$u2$udp_bw_e$pcapend1"
		done
	done
done