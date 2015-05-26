#include "algorithmbuilder.h"

#include "algorithmwander.h"
#include "algorithmtrack.h"

IAlgorithm* AlgorithmBuilder::create(int number, DataLayerBase* dataLayer)
{
  switch(number)
  {
  case 1:
      return new AlgorithmWander(dataLayer);
  case 2:
      return new AlgorithmTrack(dataLayer);
  default:
      return 0;
  }
}

