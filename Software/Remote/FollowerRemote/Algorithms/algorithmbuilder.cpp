#include "algorithmbuilder.h"

#include "algorithmwander.h"
#include "algorithmtrack.h"
#include "algorithmturn.h"

IAlgorithm* AlgorithmBuilder::create(int number, DataLayerBase* dataLayer)
{
  switch(number)
  {
  case 1:
      return new AlgorithmWander(dataLayer);
  case 2:
      return new AlgorithmTrack(dataLayer);
  case 3:
      return new AlgorithmTurn(dataLayer);
  default:
      return 0;
  }
}

