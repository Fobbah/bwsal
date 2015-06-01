#include "OffenseManager.h"

using namespace BWAPI;

namespace BWSAL {

	void OffenseManager::onFrame()
	{
		if (Broodwar->getFrameCount() % 80 == 0)
		{
			Think();
		}
		if (m_attacking)
		{
			Broodwar->drawCircle(CoordinateType::Map, lastTarget.x, lastTarget.y, 80, Colors::Red, false);
		}

		for (Unit u : mTargets)
		{
			BWAPI::Position pos = m_informationManager->getLastPosition(u);
			Broodwar->drawCircle(CoordinateType::Map, pos.x, pos.y, 5, Colors::Orange, true);
		}
	}

	void OffenseManager::onUnitDiscover(BWAPI::Unit unit)
	{
		//Add the unit to the set if it is a building
		if (unit->getType().isBuilding() && unit->getPlayer()->isEnemy(Broodwar->self()))
		{
			mTargets.insert(unit);
		}
	}

	void OffenseManager::onUnitDestroy(BWAPI::Unit unit)
	{
		mTargets.erase(unit);
	}

	void OffenseManager::Think()
	{
		UnitGroup army = SelectAll()(canAttack)(canMove).not(isWorker);
		if (army.size() > 0)
		{
			printf("THINK! PushSize=%d/%d Attacking=%s TargetSetSize=%u\n",
				army.size(), m_pushSize, m_attacking ? "TRUE" : "FALSE", mTargets.size());
			if (m_attacking == false)
			{
				if ((int)army.size() >= this->m_pushSize)
					m_attacking = true;
			}
			if (m_attacking)
			{
				BWAPI::Position attackTarget = BWAPI::Positions::None;

				if (mTargets.size() > 0)
				{
					attackTarget = getAttackTarget();
				}
				if (!isPositionValid(attackTarget))
				{
					if (m_informationManager->getEnemyBases().size() > 0)
					{
						attackTarget = (*this->m_informationManager->getEnemyBases().begin())->getPosition();
					}
				}

				if (isPositionValid(attackTarget))
				{
					army.attack(attackTarget);
					Position armyPos = army.getCenter();
					Broodwar->drawLine(CoordinateType::Map, armyPos.x, armyPos.y, attackTarget.x, attackTarget.y, Colors::Red);
						lastTarget = attackTarget;
				}
				else
				{
					printf("can't find attack position, terminating attack\n");
					m_attacking = false;
				}
			}
		}
		else
		{
			printf("can't attack, no army.\n");
		}
	}

	BWAPI::Position OffenseManager::getAttackTarget()
	{
		printf("BEGIN_UNIT_DUMP\n");
		for (BWAPI::Unit u : mTargets)
		{
			BWAPI::Position pos = m_informationManager->getLastPosition(u);
			printf("DUMP_UNITS: %s, %d@%d\b\n", m_informationManager->getType(u).toString().c_str(), pos.x, pos.y);
		}
		for (BWAPI::Unit u : mTargets)
		{
			BWAPI::Position pos = m_informationManager->getLastPosition(u);
			if (m_informationManager->getType(u) == UnitTypes::Resource_Vespene_Geyser)
			{
				continue;
			}
			
			if (isPositionValid(pos))
			{
				printf("ATTACKING UNIT:%s, %d@%d\n", m_informationManager->getType(u).toString().c_str(), pos.x, pos.y);
				return pos;
			}
		}
		return BWAPI::Positions::None;
	}

}