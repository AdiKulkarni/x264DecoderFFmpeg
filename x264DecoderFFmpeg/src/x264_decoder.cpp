//============================================================================
// Name        : x264Decoder.cpp
// Author      : Aditya Kulkarni
// Version     : 1.0
// Copyright   : Copyrighted software by the author
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "udp_client_server.h"
#include "decoder.h"
#include "rtsp.h"
#include "tcp_client.h"

#define MAX_UDP_PKT_LEN 1400
#define MAX_TCP_RECV_BUFF_LEN 1400
#define CONNECTION_UDP 0
#define CONNECTION_TCP 1
#define CONNECTION_RTP 2
#define CONNECTION_FILE 3

int main(int argc, char**argv) {

    if (argc != 4) {
        std::cout
                << "Usage: x264Decoder <Connection_Type: 0:UDP, 1:TCP, 2:RTP, 3:FILE> <Server_IP> <Server_Port> "
                << std::endl;
        exit(1);
    }

    /*buffer to send and receive data*/
    char* sendRequest = "Request From Client\n";
    char* dataToDecoder = (char*) malloc(INBUF_SIZE * sizeof(char *));

    /*file pointers*/
    FILE *infile;
    const char *filename =
            "/home/aditya/workspace/x264DecoderAll/src/incoming.h264";

    /*connection parameters*/
    int length = 0, connection = atoi(argv[1]);
    const std::string serverAddr = argv[2];
    int serverPort = atoi(argv[3]);
    /**********************
     * UDP: serverPort
     * TCP: serverPort + 1
     * RTP: serverPort + 2
     *********************/

    /*initialize decoder*/
    decoder *myDecoder = new decoder();
    myDecoder->initialize();

    switch (connection) {

            case CONNECTION_UDP:
                /*UDP stream*/{
                udp_client_server::udp_client udpClient(serverAddr, serverPort);
                udpClient.send(sendRequest, MAX_UDP_PKT_LEN);
                std::cout << "Server is " << udpClient.get_addr() << ":"
                        << udpClient.get_port() << " and Socket is "
                        << udpClient.get_socket() << std::endl;

                while (true) {
                    //directly send data to decoder as it comes without buffering
                    length = udpClient.receive(dataToDecoder, MAX_UDP_PKT_LEN);
                    myDecoder->decodeFrame(dataToDecoder, length);
                }
            }
                break;

            case CONNECTION_TCP:
                /*TCP stream*/{
                int sizeReceived = 0;
                tcp_client tcpClient;
                tcpClient.conn(serverAddr, serverPort + 1);
                tcpClient.send_data(sendRequest, MAX_TCP_RECV_BUFF_LEN);

                while (true) {
                    sizeReceived = tcpClient.receive(dataToDecoder);
                    if (sizeReceived)
                        myDecoder->decodeFrame(dataToDecoder, sizeReceived);
                }
            }
                break;

            /*case CONNECTION_RTP:
                RTSP stream{
                av_register_all();
                AVPacket* packet;
                AVFormatContext* context = avformat_alloc_context();
                rtsp rtspInstance(context);

                if (rtspInstance.init(serverAddr, serverPort + 2) == 1) {
                    std::cout << "Error in RTSP" << std::endl;
                    return 0;
                }

                avcodec_get_context_defaults3(myDecoder->codecContext,
                        myDecoder->codec);
                avcodec_copy_context(myDecoder->codecContext,
                        context->streams[rtspInstance.getVideoStreamIndex()]->codec);

                while (av_read_frame(rtspInstance.getRTSPContext(), packet) >= 0) {
                    myDecoder->decodeFrame((char*) packet, packet->size);
                    av_free_packet(packet);
                    av_init_packet(packet);
                }
                rtspInstance.close();
            }
                break;*/

            case CONNECTION_FILE:
                /*File stream*/{
                infile = fopen(filename, "rb");
                if (!infile) {
                    fprintf(stderr, "Could not open %s\n", filename);
                    exit(1);
                }

                while (true) {
                    int sizeReceived = fread(dataToDecoder, 1, INBUF_SIZE, infile);
                    if (sizeReceived > 0)
                        myDecoder->decodeFrame(dataToDecoder, sizeReceived);
                    else
                        break;
                }
                fclose(infile);
            }
                break;

            default:
                std::cout << "No stream specified. Exiting." << std::endl;
                break;
    }

    myDecoder->close();
    std::cout << "Finished decoding" << std::endl;
    return 0;
}
