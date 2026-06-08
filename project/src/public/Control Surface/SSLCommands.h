#pragma once
#include "Control Surface/ControlSurface.h"

namespace ControlSurface
{
	namespace SSL
	{
		namespace Effects
		{

			typedef struct FEffectParameter
			{
				uint32 ParameterID = 0;;
				double ParameterValue = 0.f;
				double MinimumRealValue = 0.f;
				double MaximumRealValue = 0.f;

			public:
				FEffectParameter( uint32 id ) : ParameterID( id ) {};
			};

			const double Param_Min = 0.f;
			const double Param_Max = 1.f;

			namespace NCS2
			{
				inline FEffectParameter Gain_Input( 1563454188 );
				inline FEffectParameter Gain_Output( 867502030 );
				inline FEffectParameter ChannelActive( 2004703496 );

				namespace Filters
				{
					inline FEffectParameter LowPassFilter( 1473943933 );
					inline FEffectParameter HighPassFilter( 225137163 );
				}
				namespace Eq
				{
					inline FEffectParameter Enabled( 2166545 );
					inline FEffectParameter E_Model_Enabled( 69881 );

					inline FEffectParameter HighShelf_Gain( 1240660653 );
					inline FEffectParameter HighShelf_Freq( 1240647078 );
					inline FEffectParameter HighShelf_Bell( 1240515633 );

					inline FEffectParameter HighMidBand_Gain( 1015610129 );
					inline FEffectParameter HighMidBand_Freq( 1015596554 );
					inline FEffectParameter HighMidBand_Q( 1463142943 );

					inline FEffectParameter LowMidBand_Gain( 1073914335 );
					inline FEffectParameter LowMidBand_Freq( 1073900760 );
					inline FEffectParameter LowMidBand_Q( 794052113 );

					inline FEffectParameter LowShelf_Gain( 571569823 );
					inline FEffectParameter LowShelf_Freq( 571556248 );
					inline FEffectParameter LowShelf_Bell( 571424803 );
				}
				namespace Dynamics
				{

					inline FEffectParameter Compressor_FastAttack_Enabled( 76418643 );
					inline FEffectParameter Compressor_Peak_Enabled( 1611847854 );
					inline FEffectParameter Compressor_Enabled( 1708054649 );
					inline FEffectParameter Compressor_Ratio( 576905756 );
					inline FEffectParameter Compressor_Threshold( 589759996 );
					inline FEffectParameter Compressor_Release( 462649816 );

					inline FEffectParameter Expander_Enabled( 1299822290 );
					inline FEffectParameter Expander_FastAttack_Enabled( 1357033743 );
					inline FEffectParameter Expander_Range( 615146258 );
					inline FEffectParameter Expander_Threshold( 76880832 );
					inline FEffectParameter Expander_Release( 710160540 );
					inline FEffectParameter Expander_Hold( 643021898 );
				}
			}

			namespace FourKE
			{
				namespace Filters
				{
					inline FEffectParameter LowPassFilter( 1473943933 );
					inline FEffectParameter HighPassFilter( 225137163 );
				}
				namespace Eq
				{
					inline FEffectParameter Enabled( 2166545 );
					inline FEffectParameter E_Model_Enabled( 69881 );

					inline FEffectParameter HighShelf_Gain( 1240660653 );
					inline FEffectParameter HighShelf_Freq( 1240647078 );
					inline FEffectParameter HighShelf_Bell( 1240515633 );

					inline FEffectParameter HighMidBand_Gain( 1015610129 );
					inline FEffectParameter HighMidBand_Freq( 1015596554 );
					inline FEffectParameter HighMidBand_Q( 1463142943 );

					inline FEffectParameter LowMidBand_Gain( 1073914335 );
					inline FEffectParameter LowMidBand_Freq( 1073900760 );
					inline FEffectParameter LowMidBand_Q( 794052113 );

					inline FEffectParameter LowShelf_Gain( 571569823 );
					inline FEffectParameter LowShelf_Freq( 571556248 );
					inline FEffectParameter LowShelf_Bell( 571424803 );
				}
				namespace Dynamics
				{

					inline FEffectParameter Compressor_FastAttack_Enabled( 76418643 );
					inline FEffectParameter Compressor_Peak_Enabled( 1611847854 );
					inline FEffectParameter Compressor_Enabled( 1708054649 );
					inline FEffectParameter Compressor_Ratio( 576905756 );
					inline FEffectParameter Compressor_Threshold( 589759996 );
					inline FEffectParameter Compressor_Release( 462649816 );

					inline FEffectParameter Expander_Enabled( 1299822290 );
					inline FEffectParameter Expander_FastAttack_Enabled( 1357033743 );
					inline FEffectParameter Expander_Range( 615146258 );
					inline FEffectParameter Expander_Threshold( 76880832 );
					inline FEffectParameter Expander_Release( 710160540 );
					inline FEffectParameter Expander_Hold( 643021898 );
				}
			}

			namespace FourKG
			{
				namespace Filters
				{
					inline FEffectParameter LowPassFilter( 1473943933 );
					inline FEffectParameter HighPassFilter( 225137163 );
				}
				namespace Eq
				{
					inline FEffectParameter Enabled( 2166545 );
					inline FEffectParameter E_Model_Enabled( 69881 );

					inline FEffectParameter HighShelf_Gain( 1240660653 );
					inline FEffectParameter HighShelf_Freq( 1240647078 );
					inline FEffectParameter HighShelf_Bell( 1240515633 );

					inline FEffectParameter HighMidBand_Gain( 1015610129 );
					inline FEffectParameter HighMidBand_Freq( 1015596554 );
					inline FEffectParameter HighMidBand_Q( 1463142943 );

					inline FEffectParameter LowMidBand_Gain( 1073914335 );
					inline FEffectParameter LowMidBand_Freq( 1073900760 );
					inline FEffectParameter LowMidBand_Q( 794052113 );

					inline FEffectParameter LowShelf_Gain( 571569823 );
					inline FEffectParameter LowShelf_Freq( 571556248 );
					inline FEffectParameter LowShelf_Bell( 571424803 );
				}
				namespace Dynamics
				{
					inline FEffectParameter Compressor_FastAttack_Enabled( 76418643 );
					inline FEffectParameter Compressor_Peak_Enabled( 1611847854 );
					inline FEffectParameter Compressor_Enabled( 1708054649 );
					inline FEffectParameter Compressor_Ratio( 576905756 );
					inline FEffectParameter Compressor_Threshold( 589759996 );
					inline FEffectParameter Compressor_Release( 462649816 );

					inline FEffectParameter Expander_Enabled( 1299822290 );
					inline FEffectParameter Expander_FastAttack_Enabled( 1357033743 );
					inline FEffectParameter Expander_Range( 615146258 );
					inline FEffectParameter Expander_Threshold( 76880832 );
					inline FEffectParameter Expander_Release( 710160540 );
					inline FEffectParameter Expander_Hold( 643021898 );
				}
			}

			namespace BusCompressor
			{
				inline FEffectParameter Enabled( 268180727 );
				inline FEffectParameter MixPercentage( 1065371265 );
				inline FEffectParameter Threshold( 458490955 );
				inline FEffectParameter MakeUpGain( 1072382024 );
				inline FEffectParameter Attack( 1971575400 );
				inline FEffectParameter Release( 607764455 );
				inline FEffectParameter Ratio( 78733291 );
				inline FEffectParameter SidechainHPF( 1362307156 );
			}
		}
	}

}
