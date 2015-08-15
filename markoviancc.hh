#ifndef MARKOVIANCC_HH
#define MARKOVIANCC_HH 

#include <chrono>
#include <limits>
#include <map>
#include <queue>
#include <vector>

#include "ccc.hh"
#include "exponential.hh"
#include "utilities.hh"

class MarkovianCC : public CCC { 
	double delta;

	// some adjustable parameters
	const double alpha_rtt = 1.0/16.0;
	const double alpha_intersend = 1.0/16.0;
	// set of all unacked pkts Format: (seq_num, sent_timestamp)
	//
	// Note: a packet is assumed to be lost if a packet with a higher
	// sequence number is acked. This set contains only the packets 
	// which are NOT lost
	std::map<int, double> unacknowledged_packets;
	
	TimeEwma rtt_acked_ewma;  // estimated using only acked packets
	TimeEwma rtt_unacked_ewma; // unacked packets are also considered
	TimeEwma intersend_ewma;
	// send time of previous ack. Used to calculate intersend time
	double prev_ack_sent_time;

	PRNG prng;

	// cur_time is measured relative to this
	std::chrono::high_resolution_clock::time_point start_time_point;

	// a pkt is considered lost if a gap appears in acks
	unsigned int num_pkts_lost;
	// to calculate % lost pkts
	unsigned int num_pkts_acked;

	// return a timestamp in milliseconds
	double current_timestamp();

	// update intersend time based on rtt ewma and intersend ewma
	void update_intersend_time();

public:
	MarkovianCC( ) 
	: 	CCC(), 
		delta(),
		unacknowledged_packets(),
		rtt_acked_ewma(alpha_rtt),
		rtt_unacked_ewma(alpha_rtt),
		intersend_ewma(alpha_intersend),
		prev_ack_sent_time(),
		prng(current_timestamp()),
		start_time_point(),
		num_pkts_lost(),
		num_pkts_acked()
	{}

	// callback functions for packet events
	virtual void init() override;
	virtual void onACK(int ack, double receiver_timestamp) override ;
	virtual void onPktSent(int seq_num) override ;
	virtual void onTimeout() override { std::cerr << "Ack timed out!\n"; }
	virtual void onLinkRateMeasurement( double s_measured_link_rate ) override;
};

#endif