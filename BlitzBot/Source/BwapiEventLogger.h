#pragma once
#include <BWAPI.h>

namespace BWSAL {

	class BwapiEventLogger : public BWAPI::AIModule
	{
		void onStart() override;
		void onEnd(bool isWinner) override;
		void onFrame() override;
		void onNukeDetect(BWAPI::Position target) override;
		void onPlayerLeft(BWAPI::Player player) override;
		void onReceiveText(BWAPI::Player player, std::string text);
		void onSendText(std::string text) override;
		void onUnitDiscover(BWAPI::Unit unit) override;
		void onUnitHide(BWAPI::Unit unit) override;
		void onUnitShow(BWAPI::Unit unit) override {}
		void onUnitEvade(BWAPI::Unit unit) override;
		void onUnitDestroy(BWAPI::Unit unit) override;
		void onUnitMorph(BWAPI::Unit unit) override;
		void onUnitRenegade(BWAPI::Unit unit) override;
		void onUnitComplete(BWAPI::Unit unit) override;
	};
}