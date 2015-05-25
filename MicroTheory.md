# Optimal micro strategies for two opposing units #

Let **A** and **B** denote two opposing units in a real-time strategy game where each unit has 5 fixed properties: _speed_, _range_, _dps_ (damage per shot), and _cooldown_. Each unit also has 2 dynamic properties: _position_ (a vector in R<sup>n</sup>) and _health_, a scalar. We also assume that the initial distance between both units is greater than the range of either unit. That is, |**A**._position_-**B**._position_| > max(**A**._range_,**B**._range_).

Let shots(**X**,**Y**) denote the number of shots it takes for **X** to kill **Y**. Since each shot by **X** reduces **Y**'s _health_ by **X**._dps_, we can define shots(**X**,**Y**) as follows:

shots(**X**,**Y**) = ceil(**Y**._health_/**X**._dps_)

Let timeToKill(**X**,**Y**) denote the time it takes for **X** to kill **Y**, assuming |**X**.position-**Y**.position|<**X**._range_ for the entire time:

timeToKill(**X**,**Y**) = (shots(**X**,**Y**)-1)`*`**X**._cooldown_

**A**._range_ can be less than, equal to, or greater than **B**._range_, and **A**._speed_ can be less than, equal to, or greater than **B**._speed_, so there are 9 possible cases when considering both of them which we will group into the following 4 cases:

  1. **A**._speed_ > **B**._speed_ and **A**._range_ > **B**._range_
  1. **A**._speed_ > **B**._speed_ and **A**._range_ <= **B**._range_
  1. **A**._speed_ = **B**._speed_
  1. **A**._speed_ < **B**._speed_

## **A**._speed_ > **B**._speed_ and **A**._range_ > **B**._range_ ##

In this case, **A** can always win regardless of the other unit properties. Since  **A**._speed_ > **B**._speed_, it can control whether |**A**._position_-**B**._position_| increases, decreases, or stays the same at each point in time. Furthermore, since **A**._range_ > **B**._range_, **A** can move toward **B** until **A**.range > |**A**.position-**B**.position| > **B**.range, and then maintain that distance while it attacks **B**. At this distance **B** cannot attack **A**, and since **B** is slower than **A**, it cannot get close enough to attack **A** nor evade **A**, so **A** always wins. This is commonly called "kiting".

## **A**._speed_ > **B**._speed_ and **A**._range_ <= **B**._range_ ##

In this case, **A** can move faster than **B**, but it does not have a greater range. However **A** can still kill **B** if the time it takes for **A** to kill **B** plus the time it takes for **A** to move from a distance of |**A**._position_-**B**._position_| = **B**._range_ to a distance of |**A**._position_-**B**._position_| = **A**._range_ is less than the time it takes for **B** to kill **A**. The time it takes for **A** to move a distance of **B**._range - **A**._range_is (**B**._range_-**A**._range)/**A**._speed_, however we should assume that **B** will try to maximize the amount of time it can attack **A** without **A** being able to attack **B**, so we use the time (**B**._range_-**A**._range)/(**A**._speed_-**B**._speed). Our condition can then be written as:

timeToKill(**A**,**B**) + (**B**._range_-**A**._range)/(**A**._speed_-**B**._speed) < timeToKill(**B**,**A**)

If this inequality is true, **A** can attack and kill **B**.

If not, **A** may still be able to kill **B**, not in a straight up attack, but by quickly micro'ing in and out of range of **B**. This strategy only makes sense to consider if the following to pre-condition holds: **A**'s cooldown is longer than the time needed to move out of **B**'s attack range and back in. If this precondition is false, then **A** will not have time to micro out of range of **B** and back in before it needs to fire its next shot, and so microing in and out of range will just reduce **A**'s attack rate. This precondition can be stated as the following inequality:

**A**._cooldown_ > 2`*`(**B**._range_-**A**._range)/(**A**._speed_-**B**._speed)

Assuming this precondition holds, we can say that the number of shots **B** can get off during each time that **A** is in **B**'s range is:

shotsPerMicroRound(**B**,**A**) = ceil((2`*`(**B**._range_-**A**._range)/(**A**._speed_-**B**._speed))/**B**._cooldown_)

where "MicroRound" is the event of **A** moving into range of **B** to attack **B** and then moving back out.

Similarly, the number of full micro rounds for **B** to kill **A** is:

fullMicroRounds(**B**,**A**) = floor(shots(**B**,**A**) / shotsPerMicroRound(**B**,**A**))

The last micro round is not full and has the following number of remaining shots:

remainingShots(**B**,**A**) = shots(**B**,**A**) % shotsPerMicroRound(**B**,**A**)

From this we can express the time it takes **B** to kill **A**, given that **A** is microing in and out of range:

adjustedTimeToKill(**B**,**A**) = fullMicroRounds(**B**,**A**) `*` A.cooldown  + remainingShots(**B**,**A**)`*` **B**._cooldown_

From this, our condition becomes:

timeToKill(**A**,**B**) + (**B**._range_-**A**._range)/(**A**._speed_-**B**._speed) < adjustedTimeToKill(**B**,**A**)

If this inequality is true, then **A** can attack and kill **B**.

## **A**._speed_ = **B**._speed_ ##

In this case neither unit can win if they are both rational - the weaker unit will always retreat, and since they can move at the same speed, the stronger unit will never be able to attack. Nonetheless, **A** should try to attack iff **A**._range_ > **B**._range_ or **A**._range_ = **B**._range_ and timeToKill(A,B) < timeToKill(B,A) . If **B**._range_ > **A**._range_, **B** should be trying to attack **A** and **A** should retreat.

## **A**._speed_ < **B**._speed_ ##

If **B** is rational, this case has only 2 outcomes - **B** chooses to attack and kill **A**, or **B** evades **A**. The decision making for **B** proceeds as it did for **A** in the first 2 cases. If **A**._range_ >= **B**._range, then **A** will be able to do damage to **B** so it should try to do as much damage to **B** before it dies._

# Heuristics for m units vs n units #

While micro'ing m units of group **A** vs n units of group **B** is NP-hard, one possible heuristic for approximately solving this would be to have group **A** micro so as to maximize timeToKill`'`(**B**,**A**) while not increasing timeToKill`'`(**A**,**B**), where timeToKill`'`(**X**,**Y**) is a heuristic generalization of timeToKill(**X**,**Y**) that works for two groups of units that are all assumed to be in range of each other.

Computing timeToKill`'`(**X**,**Y**):

Let **X** be a set of m units that all have the same _speed_, _range_, _dps_, and _cooldown_, but possibly different _health_.

Let **Y** be a set of n units that all have the same _speed_, _range_, _dps_, and _cooldown_, but possibly different _health_.

Let **Y**._health_<sub>i</sub> denote the _health_ of unit i in set **Y**.

Then we can say the number of shots for a unit in **X** to kill **Y**<sub>j</sub> is :

shots<sub>G</sub>(**X**, **Y**<sub>j</sub>) = ceil(**Y**._health_<sub>j</sub> / **X**._dps_)

From this, the number of shots for a unit in **X** to kill all of **Y** is:

shots<sub>G</sub>(**X**, **Y**) = sum(shots<sub>G</sub>(**X**, **Y**<sub>j</sub>), j=1 to m)

Since **X** has m units that can all attack at once, the time it takes for **X** to kill **Y** is:

timeToKill`'`(**X**,**Y**) = (ceil(shots<sub>G</sub>(**X**, **Y**)/m) - 1) `*` **X**._cooldown_