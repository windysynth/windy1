/* 	
*	Developed by Aidan Walton 2023.  aidan.walton@gmail.com. Based on the academic review:
*	Digital Dynamic Range Compressor Design
*	A Tutorial and Analysis
*	DIMITRIOS GIANNOULIS , MICHAEL MASSBERG , AND JOSHUA D. REISS, AES Member
*	Queen Mary University of London, London, UK
*/


#include <Arduino.h>
#include "effect_compressor.h"
#include "utility/dspinst.h"

constexpr float AudioEffectCompressor::LUT_q15_to_dB[];
constexpr int16_t AudioEffectCompressor::LUT_dB_to_3q13[];
constexpr int16_t AudioEffectCompressor::LUT_dB_to_5q11[];


void AudioEffectCompressor::sideChain(const int16_t *in, int16_t *out) { 
	// The compressor is implemented as feedforward type with Level detection in the linear domain followed by gain computation in the logdomain "Digital Dynamic Range Compressor Design Fig.7 (a)"
	// Note however that the application of makeup gain is applied within the linear domain using fixed point, instead of within the logdomain as per Fig.7(a).
	// This avoids the need to have a log_to_lin LUT from log to lenear conversion that extends positvely above 0dB.
	// Make-up gain is a user user defined variable and the conversion from its log to linear value can be done in none real-time.
	const int16_t *end = in + AUDIO_BLOCK_SAMPLES;
	int32_t input, peakDetOutput, peakDetFeedback, release_cur, release_to_input, intermediate;
	//const int16_t *dwnSamBlkStart;
	//int16_t *outBlkStart = out;
	//int16_t inLevel;
	
    	
    	uint32_t blockCnt = 1;
	
	
	do {	
		// In the linear domain we do smoothed decoupled peak detection.
		// This is running at Fs (44.1K) and is fixed point hence quite efficient.
		// y1 [n] = max(xL[n], αR*y1[n - 1] + (1 - αR )*L[n])
		input = (*in) << 16;
		input = (input > 0) ? input : signed_subtract_16_and_16(0, input); 		// Full wave rectify input signal
		if(input > release_prev) release_cur = input;					// Maximally fast peak detect
		else {										// 1st order release
			release_cur = multiply_32x32_rshift32_rounded(alphaRelease, release_prev);
			release_cur = signed_saturate_rshift(release_cur, 31,0);
			release_cur <<= 1;
			
			release_to_input = multiply_32x32_rshift32_rounded(alphaReleaseComp, input);
			release_to_input = signed_saturate_rshift(release_to_input, 31,0);
			release_to_input <<= 1;
			release_cur = signed_add_16_and_16(release_cur, release_to_input);
		}
		release_prev = release_cur;
		
		// yL[n] = αA*yL[n - 1] + (1 - αA )*y1[n]
		peakDetOutput = multiply_32x32_rshift32_rounded(release_cur, alphaAttackComp);	// 1st order attack
		peakDetOutput = signed_saturate_rshift(peakDetOutput, 31,0);
		peakDetOutput <<= 1;
		peakDetFeedback = multiply_32x32_rshift32_rounded(peakDet_prev, alphaAttack);
		peakDetFeedback = signed_saturate_rshift(peakDetFeedback, 31,0);
		peakDetFeedback <<= 1;
		peakDetOutput = signed_add_16_and_16(peakDetOutput, peakDetFeedback);
		peakDet_prev = peakDetOutput;
		
			
		
		

		// We can downsample the logDomain as it is computed using floats and can be quite a hog on fixed point only MCUs.
		// This determines the rate at which compressor gain adjustments are calculated and applied. 
		// The stream rate and level detect remain at Fs, but 'downSample' sized blocks all have the same compression gain applied.
		// Under typical use cases the change between compression gain steps are quite small, therefore even if the
		// compute runs at at 0.25Fs (downSample = 4) or even 0.125Fs (downSample = 8), it still results in subjectivly acceptable quality audio.
		// Although perhaps not ideal, it is possible to choose a downsample rate which is not 2^n divisor of Fs.
		// The result is simply a final atten block with more or less total samples than others within the same AUDIO_BLOCK_SAMPLES. 
		if((blockCnt == downSample) | (downSample == 1)) {
			float gain_dB = 0;
			float IN_dB;
			float int_part;
		    	float fract_part;
		    	float fract_part_scaled;
		    	uint32_t GAIN_5q11;
		    	uint32_t GAIN_3q13;
			
			
			// Compute compression:
			// Where yG = desired output level & xG is input level at the compressor
			// To calculate gain attenuation (comp_gain_dB)  = xG - yG
			//     
			//                      T + (xG - T )/R                         2(xG - T ) > W          (Above Knee)
			// yG =      xG + (1/R - 1)(xG - T + W/2)^2/(2W)                2/|(xG - T )|/ ≤ W      (In the Knee)
			//                          xG                                  2(xG - T ) < -W         (Below Knee)
			
			
			IN_dB = LUT_q15_to_dB[signed_saturate_rshift(peakDetOutput, 16, 20)];  // The signed_saturate_rshift using 20 bits takes care of the required >> 4 of a q1.15 into the LUT 
				
			
			if((2*(IN_dB - thresh)) > knee) {
				gain_dB = -(IN_dB - (thresh + ((IN_dB - thresh) / ratio)));
			} else if ((2*abs(IN_dB - thresh)) <= knee)  {
				gain_dB = -(IN_dB - (IN_dB + ratio_resip * ((IN_dB - thresh + knee_div_2) * (IN_dB - thresh + knee_div_2)) / knee_x_2));
			} else {
				gain_dB = 0;
			}
				

			// Convert from Log domain to Linear domain.
			// This uses 2 distinct LUTs. 
			// The first covers the input range from 0 through -8dB and has a higher resolution and uses a q3.13 format.
			// The second range from -8 through -40 has less resolution and uses a q5.11 format.
			// The aim here is to reduce overall storage space required to achieve highspeed log2lin conversion.
			
			if(gain_dB <= -40) gain_dB = -39.99999;                                 // Restrict range to within LUTs range
		
			if(gain_dB == 0) atten = 0x7FFF;
			else {
			    fract_part = modff(-gain_dB, &int_part);
			    if(gain_dB > -8) {
				fract_part_scaled = 0x1FFF*fract_part;
				GAIN_3q13 = 0x0 | (uint32_t)fract_part_scaled | ((uint32_t)int_part) << 13;
				atten = LUT_dB_to_3q13[GAIN_3q13 >> 4];       // Convert the dB gain into the correct q format, then shift right to scale the result into the size (of the LUT array) of the LUT
			    } else {
				fract_part_scaled = 0x7FF*fract_part;
				GAIN_5q11 = 0x0 | (uint32_t)fract_part_scaled | ((uint32_t)(int_part - 8)) << 11;
				atten = LUT_dB_to_5q11[GAIN_5q11 >> 5];       // Convert the dB gain into the correct q format, then shift right to scale the result into the size (of the LUT array) of the LUT
			    }
			}			
			blockCnt = 1;
		}
 	
		intermediate = multiply_32x32_rshift32_rounded(makeupGain, (*in) << 16);	// Apply makeup gain to input and generate 32bit intermediate
		intermediate = multiply_32x32_rshift32_rounded(intermediate, atten << 16);	// Apply compression q1.15 attenuation and generate 32bit intermediate
		intermediate <<= 1; 
		*out++ = saturate16(intermediate);
		in++;
		++blockCnt;	
	}  while (in < end);
}



void AudioEffectCompressor::update(void)
{
	audio_block_t *input_block=NULL, *output_block=NULL;
	audio_block_t *sideChainA_block=NULL, *sideChainB_block=NULL, *sideChain_block=NULL;
	bool missingSideChain=false;

	input_block = receiveReadOnly(0);
	sideChainA_block = receiveReadOnly(1);
	sideChainB_block = receiveReadOnly(2);
	
	if(!input_block) {
		if(sideChainA_block) release(sideChainA_block);
		if(sideChainB_block) release(sideChainB_block);
		return;
	}	

	switch(sideChainSel) {
		case 0:
			if(!sideChainA_block) missingSideChain = true;
			else sideChain_block = sideChainA_block;
		break;
		case 1:
			if(!sideChainB_block) missingSideChain = true;
			else sideChain_block = sideChainB_block;
		break;
	}
	
	if(missingSideChain) {
		if(sideChainA_block) release(sideChainA_block);
		if(sideChainB_block) release(sideChainB_block);
		transmit(input_block);
		release(input_block);
		return;
	}
	
	output_block = allocate();
	if(!output_block) {
		if(sideChainA_block) release(sideChainA_block);
		if(sideChainB_block) release(sideChainB_block);
		transmit(input_block);
		release(input_block);
		return;
	}


	if(enabled) {
		sideChain(sideChain_block->data, output_block->data);
		transmit(output_block);
	}
	else transmit(input_block);
	
	release(output_block);
	release(input_block);
	if(sideChainA_block) release(sideChainA_block);
	if(sideChainB_block) release(sideChainB_block);
	
	return;
}




