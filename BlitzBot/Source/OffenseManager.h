#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL.h>
namespace BWSAL {

	class OffenseManager : public BWAPI::AIModule
	{
	public:
		OffenseManager(InformationManager* informationManager)
			: m_informationManager(informationManager)
			, m_attacking(false)
		{}

		virtual void onStart() override {}
		virtual void onEnd(bool isWinner) override {}
		virtual void onFrame() override;
		virtual void onSendText(std::string text) override {}
		virtual void onUnitDiscover(BWAPI::Unit unit) override;
		virtual void onUnitEvade(BWAPI::Unit unit) override {}
		virtual void onUnitDestroy(BWAPI::Unit unit) override;
		virtual void onUnitMorph(BWAPI::Unit unit) override {}
		virtual void onUnitRenegade(BWAPI::Unit unit) override {}
		virtual void onUnitComplete(BWAPI::Unit unit) override {}

		void setPushSize(int size)
		{
			m_pushSize = size;
		}

	private:
		InformationManager* m_informationManager;
		bool m_attacking;
		int m_pushSize;
		BWAPI::Unitset mTargets;

		void Think();

		BWAPI::Position getAttackTarget();

		BWAPI::Position lastTarget;

		bool isPositionValid(BWAPI::Position pos)
		{
			return pos != BWAPI::Positions::Invalid && 
				pos != BWAPI::Positions::None && 
				pos != BWAPI::Positions::Unknown;
		}
	};
}