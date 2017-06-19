#!/bin/bash

# Bash zum Ausführen meherer Tests

# Config Variables #
output="Auswertung/"	# Pfad zu den Auswertungen
path="scratch/"			# Pfad in dem das TestSkript liegt
program="tu_bench_955625"	# Name des TestSkript
pcapstart="bench_"		# Bezeichnung für die Ausgabedatein
pcapend1=".pcap"		# Dateiformat der endgültigen Ausgaben

# Vorgaben des Formats für die Abgabe
splitter1="_"	# Einfacher Trenner
splitter2="_U" # Spezieller Trenner vor der Datenrate

# TCP Typen, welche getestet werden sollen
tcp_types=("Rfc793" "TcpNewReno" "TcpReno" "TcpTahoe" "TcpWestwood" "TcpWestwoodPlus")

# Zu testende Geschwindigkeiten
udp_bws=("0Mbps" "1Mbps" "2Mbps")

# Zu testende Queue Implenentationen
queues=("RED" "DropTail")

# Erstelle Pfad zu den Auswertungen
mkdir -p $output

# Ausführen aller Tests für:

# Alle TCP Typen, sowie für
for tcp_type in "${tcp_types[@]}"
do
	# Alle Queue Implementationen, sowie für
	for queue in "${queues[@]}"
	do
		# Für alle Geschwindigkeiten
		for udp_bw in "${udp_bws[@]}"
		do
			echo "TCP[$tcp_type] \n Queue[$queue] \n Geschwindigkeit[$udp_bw]" # kurze Status Ausgabe
			mkdir -p "$output/$tcp_type-$queue" # Erstelle Verzeichnis für den TCP Typ differenziert nach Queue Implentation
			
			# # # # # # #
			# Herzstück #
			# # # # # # #

			# Führe TestSkript aus, mit übergabe der Parameter der aktuellen Situation
			#            Programm       TCP Typ               Queue Typ        Datenrate      Mit PcaP_Aufzeichnung    Log
			./waf --run "$program --tcp_type=$tcp_type --queue=$queue --udp_bw=$udp_bw --pcap_tracing=true --log_level_enabled=false" --cwd $output
			echo "Move: $output$pcapstart$tcp_type$splitter1$queue$splitter2$udp_bw-3-0$pcapend1  -->  $output$tcp_type-$queue/$pcapstart$tcp_type$splitter1$queue$splitter2$udp_bw$pcapend1"
			mv "$output$pcapstart$tcp_type$splitter1$queue$splitter2$udp_bw-3-0$pcapend1" "$output$tcp_type-$queue/$pcapstart$tcp_type$splitter1$queue$splitter2$udp_bw$pcapend1"
		done
	done
done