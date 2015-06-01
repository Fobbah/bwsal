#include "BwapiEventLogger.h"
#include <BWAPI.h>
#include "bwlog.h"

namespace BWSAL {

	void BwapiEventLogger::onStart()
	{
		BWLOG(logINFO) << "[EVENT][onStart]";
	}

	void BwapiEventLogger::onEnd(bool isWinner)
	{
		BWLOG(logINFO) << "[EVENT][onEnd] IsWinner=" << isWinner ? "TRUE" : "FALSE";
	}

	void BwapiEventLogger::onFrame()
	{
		//BWLOG(logINFO) << "[EVENT][onFrame]";
	}

	void BwapiEventLogger::onSendText(std::string text)
	{
		BWLOG(logINFO) << "[EVENT][onSendText] Text=" << text;
	}

	void BwapiEventLogger::onUnitDiscover(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitDiscover] UnitID=" << unit->getID()
			<< " Type=" << unit->getType().toString()
			<< " Position=" << unit->getPosition().x
			<< "@" << unit->getPosition().y;
	}

	void BwapiEventLogger::onUnitEvade(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitEvade] UnitID=" << unit->getID();
	}

	void BwapiEventLogger::onUnitHide(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitHide] UnitID=" << unit->getID();
	}

	void BwapiEventLogger::onUnitDestroy(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitDestroy] UnitID=" << unit->getID()
			<< " Type=" << unit->getType().toString()
			<< " Position=" << unit->getPosition().x
			<< "@" << unit->getPosition().y;
	}

	void BwapiEventLogger::onUnitMorph(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitMorph] UnitID=" << unit->getID()
			<< " NewType=" << unit->getType().toString()
			<< " Position=" << unit->getPosition().x
			<< "@" << unit->getPosition().y;
	}

	void BwapiEventLogger::onUnitRenegade(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitRenegade] UnitID=" << unit->getID();
	}

	void BwapiEventLogger::onUnitComplete(BWAPI::Unit unit)
	{
		BWLOG(logINFO) << "[EVENT][onUnitComplete] UnitID=" << unit->getID()
			<< " Type=" << unit->getType().toString()
			<< " Position=" << unit->getPosition().x
			<< "@" << unit->getPosition().y;
	}

	void BwapiEventLogger::onNukeDetect(BWAPI::Position target)
	{
		BWLOG(logINFO) << "[EVENT][onNukeDetect] TargetPosition=" << target.x
			<< "@" << target.y;
	}

	void BwapiEventLogger::onPlayerLeft(BWAPI::Player player)
	{
		BWLOG(logINFO) << "[EVENT][onPlayerLeft] PlayerID=" << player->getID();
	}

	void BwapiEventLogger::onReceiveText(BWAPI::Player player, std::string text)
	{
		BWLOG(logINFO) << "[EVENT][onReceiveText] PlayerID=" << player->getID()
			<< " Text=" << text;
	}

}