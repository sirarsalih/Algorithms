chan ab = [1] of {int};
chan cd = [1] of {int};
chan ef = [1] of {int};
chan gh = [1] of {int};

bool leader = false;

proctype InitializationProcess(chan outChannel; int myID)
{
	outChannel!myID;
}

proctype Process(chan inChannel; chan outChannel; int myID)
{
	int incomingID;
	outChannel?incomingID;
	
	do	
	::
		if
		:: (leader == true) -> (incomingID > myID) -> goto exit;

		:: (incomingID > myID) -> outChannel!incomingID -> break;

		:: (incomingID < myID) -> outChannel!myID -> break;

		:: (incomingID == myID) -> leader = true; outChannel!incomingID; printf("Leader is: %d", incomingID) -> break;
		fi
	
	od;

	exit:
	printf("Leader found. Exiting...");
}


init
{
	atomic 
	{
		run InitializationProcess(ab, 1); run InitializationProcess(cd, 2);
		run InitializationProcess(ef, 3); run InitializationProcess(gh, 4);
	};
	
	atomic
	{
		run Process(gh, ab, 1); run Process(ab, cd, 2);
		run Process(cd, ef, 3); run Process(ef, gh, 4); 			
	};
}
