#include "GameScene.h"
#include "MainScene.h"
// #include "Highscores.h"

//  Bonus prize Images
const char * bonus_image[]=
{
	"5.png",
	"10.png",
	"50.png",
	"100.png"
};

// Initialize the GameScene
bool GameScene::init()
{
	CCLog("GameScene::init");
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////
        CC_BREAK_IF(! CCLayer::init());

		// Initialize the parent
		MainScene::init();

		// Start off as game suspended
		gameSuspended = true;

		// Get the bird sprite
		CCSprite *bird = CCSprite::spriteWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("bird.png"));
		this->addChild(bird, 4, kBird);

		// Initialize the platforms
		initPlatforms();

		// Create the bonus sprite
		CCSprite *bonus;

		// Load in the bonus images, 5, 10, 50, 100
		for(int i=0; i<kNumBonuses; i++) 
		{
			bonus = CCSprite::spriteWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(bonus_image[i]));
			this->addChild(bonus,4, kBonusStartTag+i);
			bonus->setIsVisible(false);
		}

		// Create the Score Label
		CCLabelBMFont* scoreLabel = CCLabelBMFont::bitmapFontAtlasWithString("0",  "Images/bitmapFont.fnt");
		this->addChild(scoreLabel, 5, kScoreLabel);

		// Center the label
		scoreLabel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2,CCDirector::sharedDirector()->getWinSize().height - 50));

		// Start the GameScene stepping
		schedule(schedule_selector(GameScene::step));

		// Enable the touch events
		setIsTouchEnabled(true);
		// Enable accelerometer events
		setIsAccelerometerEnabled(true);

		// Start the game
		startGame();

        bRet = true;
    } while (0);

    return bRet;
}


// Initialize the platforms that the bird jumps on
void GameScene::initPlatforms(void)
{
	CCLog("initPlatforms");
	
	currentPlatformTag = kPlatformsStartTag;
	while(currentPlatformTag < kPlatformsStartTag + kNumPlatforms) 
	{
		initPlatform();
		currentPlatformTag++;
	}
	
	resetPlatforms();
}

// Supports Two types of platforms although only one exists in the art.
void GameScene::initPlatform(void)
{
	CCSprite *platform;

	switch(rand()%2) 
	{
		case 0: 
			platform = CCSprite::spriteWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("platform1.png"));
			this->addChild(platform, 3, currentPlatformTag);

			break;
		case 1: 
			platform = CCSprite::spriteWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("platform2.png"));
			this->addChild(platform, 3, currentPlatformTag);
			break;
	}
}

// Initializes everything and then starts the game by setting the gameSuspend
void GameScene::startGame(void)
{
	CCLog("startGame");

	score = 0;
	
	resetClouds();
	resetPlatforms();
	resetBird();
	resetBonus();
	
	gameSuspended = false;
}

// Reset the platforms as they move off the screen
void GameScene::resetPlatforms(void)
{
	CCLog("resetPlatforms");
	
	currentPlatformY = -1;
	currentPlatformTag = kPlatformsStartTag;
	currentMaxPlatformStep = 60.0f;
	currentBonusPlatformIndex = 0;
	currentBonusType = 0;
	platformCount = 0;

	while(currentPlatformTag < kPlatformsStartTag + kNumPlatforms) 
	{
		resetPlatform();
		currentPlatformTag++;
	}
}


void GameScene::resetPlatform(void)
{	
	if(currentPlatformY < 0) 
	{
		currentPlatformY = 30.0f;
	} 
	else 
	{
		currentPlatformY += rand() % (int)(currentMaxPlatformStep - kMinPlatformStep) + kMinPlatformStep;
		if(currentMaxPlatformStep < kMaxPlatformStep) 
		{
			currentMaxPlatformStep += 0.5f;
		}
	}
	
	CCSprite *platform = (CCSprite*)getChildByTag(currentPlatformTag);

	if(rand() % 2 == 1) platform->setScaleX(-1.0f);

	float x;
	CCSize size = platform->getContentSize();

	if(currentPlatformY == 30.0f) 
	{
		x = (float) CCDirector::sharedDirector()->getWinSize().width/2;
	} 
	else 
	{
		x = rand() % (int)(CCDirector::sharedDirector()->getWinSize().width -(int)size.width) + size.width/2;
	}
	
	platform->setPosition(ccp(x,currentPlatformY));
	platformCount++;

	if(platformCount == currentBonusPlatformIndex) 
	{
		CCLog("platformCount = %d",platformCount);
		CCSprite *bonus = (CCSprite*)getChildByTag(kBonusStartTag+currentBonusType);
		bonus->setPosition(ccp(x, currentPlatformY+30));
		bonus->setIsVisible(true);
	}
}

// Reset the bird to its home location
void GameScene::resetBird(void)
{
	CCLog("resetBird");

	CCSprite *bird = (CCSprite*)getChildByTag(kBird);

	bird_pos.x = (float) CCDirector::sharedDirector()->getWinSize().width/2;
	bird_pos.y = (float) CCDirector::sharedDirector()->getWinSize().width/2;
	bird->setPosition(bird_pos);
	
	bird_vel.x = 0;
	bird_vel.y = 0;
	
	bird_acc.x = 0;
	bird_acc.y = -550.0f;
	
	birdLookingRight = true;
	bird->setScaleX(1.0f);
}

// Reset the bonus types based on the current score
void GameScene::resetBonus(void)
{
	CCLog("resetBonus");
	
	CCSprite *bonus = (CCSprite*)getChildByTag(kBonusStartTag+currentBonusType);

	bonus->setIsVisible(false);
	currentBonusPlatformIndex += rand() % (kMaxBonusStep - kMinBonusStep) + kMinBonusStep;
	if(score < 10000) 
	{
		currentBonusType = 0;
	} 
	else if(score < 20000) 
	{
		currentBonusType = rand() % 2;
	} else if(score < 10000) 
	{
		currentBonusType = rand() % 3;
	} else 
	{
		currentBonusType = rand() % 2 + 2;
	}
}

/////////////////////////////////////////////////////////
// Main game loop

void GameScene::step(ccTime dt)
{
//	CCLog("Game::step");

	// Return if game suspended
	if(gameSuspended) return;

	// Get the bird sprite
	CCSprite *bird = (CCSprite*)getChildByTag(kBird);
	
	// Update the player x position based on velocity and delta time
	bird_pos.x += bird_vel.x * dt;
	
	// Flip the player based on it's x velocity and current looking direction
	if(bird_vel.x < -30.0f && birdLookingRight) 
	{
		birdLookingRight = false;
		bird->setScaleX(-1.0f);
	} else if (bird_vel.x > 30.0f && !birdLookingRight) 
	{
		birdLookingRight = true;
		bird->setScaleX(1.0f);
	}

	// Calculate the max and min x values for the player
	// based on the screen and player widths
	CCSize bird_size = bird->getContentSize();
	float max_x = (float)CCDirector::sharedDirector()->getWinSize().width - bird_size.width/2;
	float min_x = bird_size.width/2;
	
	// Limit the player position based on max and min values allowed
	if(bird_pos.x>max_x) bird_pos.x = max_x;
	if(bird_pos.x<min_x) bird_pos.x = min_x;

	// Update the player velocity based on acceleration and time
	bird_vel.y += bird_acc.y * dt;

	// Update the player y positin based on velocity and time
	bird_pos.y += bird_vel.y * dt;
	
	////////////////////////////////////////////////////////////////////////////
	// Handle the bonus scoring
	CCSprite *bonus = (CCSprite*)getChildByTag(kBonusStartTag+currentBonusType);

	// If bonus is visible then see if the bird is within range to get the bonus
	if(bonus->getIsVisible()) 
	{
		CCPoint bonus_pos = bonus->getPosition();
		float range = 20.0f;

		// If the player is within range of the bonus value then award the prize
		if(bird_pos.x > bonus_pos.x - range &&
		   bird_pos.x < bonus_pos.x + range &&
		   bird_pos.y > bonus_pos.y - range &&
		   bird_pos.y < bonus_pos.y + range ) 
		{
			// Update score based on bonus
			switch(currentBonusType) 
			{
				case kBonus5:   score += 5000;   break;
				case kBonus10:  score += 10000;  break;
				case kBonus50:  score += 50000;  break;
				case kBonus100: score += 100000; break;
			}

			// Build the score string to display
			char scoreStr[10] = {0};
			sprintf(scoreStr, "%d", score);
			CCLabelBMFont* scoreLabel = (CCLabelBMFont*) getChildByTag(kScoreLabel);
			scoreLabel->setString(scoreStr);

			// Highlight the score with some actions to celebrate the bonus win
			CCActionInterval* a1 = CCScaleTo::actionWithDuration(0.2f, 1.5f, 0.8f);
			CCActionInterval* a2 = CCScaleTo::actionWithDuration(0.2f, 1.0f, 1.0f);
			scoreLabel->runAction(CCSequence::actions(a1, a2, a1, a2, a1, a2, NULL));

			resetBonus();
		}
	}

	// If the bird has stopped moving then make it jump from the platform it is on
	int t;
	if(bird_vel.y < 0) 
	{
		t = kPlatformsStartTag;
		for(t; t < kPlatformsStartTag + kNumPlatforms; t++) 
		{
			CCSprite *platform = (CCSprite*)getChildByTag(t);

			CCSize platform_size = platform->getContentSize();
			CCPoint platform_pos = platform->getPosition();
			
			max_x = platform_pos.x - platform_size.width/2 - 10;
			min_x = platform_pos.x + platform_size.width/2 + 10;

			float min_y = platform_pos.y + (platform_size.height+bird_size.height)/2 - kPlatformTopPadding;
			
			if(bird_pos.x > max_x &&
			   bird_pos.x < min_x &&
			   bird_pos.y > platform_pos.y &&
			   bird_pos.y < min_y) {
				jump();
			}
		}
	
		// If the bird has fallen below the screen then game over
		if(bird_pos.y < - bird_size.height/2) 
		{
			// [self showHighscores];   <== NEED TO IMPLEMENT THE HIGHTSCORE
			resetBird();	// Highscore not implmented yet so just keep on going.
		}
		
	} 
	else if(bird_pos.y > ((float)CCDirector::sharedDirector()->getWinSize().height / 2))	// Bird is going off top of screen 
	{
		float delta = bird_pos.y - ((float)CCDirector::sharedDirector()->getWinSize().height / 2);
		bird_pos.y = (float)CCDirector::sharedDirector()->getWinSize().height / 2;

		currentPlatformY -= delta;

		// Move the clouds vertically and reset if necessary
		t = kCloudsStartTag;
		for(t; t < kCloudsStartTag + kNumClouds; t++) 
		{
			CCSprite *cloud = (CCSprite*) getChildByTag(t);

			CCPoint pos = cloud->getPosition();

			// Calculate new position for cloud
			pos.y -= delta * cloud->getScaleY() * 0.8f;

			// If the cloud is off the screen then need to reset this cloud else set its new position
			if(pos.y < -cloud->getContentSize().height/2) 
			{
				currentCloudTag = t;
				resetCloud();
			} 
			else 
			{	// Update the new y position for the cloud.
				cloud->setPosition(pos);
			}
		}


		// Move the platforms vertically and reset if necessary
		t = kPlatformsStartTag;
		for(t; t < kPlatformsStartTag + kNumPlatforms; t++) 
		{
			CCSprite *platform = (CCSprite*)getChildByTag(t);
			
			CCPoint pos = platform->getPosition();

			// Calculate new position for platform
			pos = ccp(pos.x, pos.y - delta);

			// If the platform is off the screen then reset the platform else set its new position
			if(pos.y < - platform->getContentSize().height/2) 
			{
				currentPlatformTag = t;
				resetPlatform();
			} 
			else 
			{
				platform->setPosition(pos);
			}
		}

		// If the bonus is visible then adjust it's y position
		if(bonus->getIsVisible()) 
		{
			CCPoint pos = bonus->getPosition();

			// Calculate new position of bonus
			pos.y -= delta;
			
			// If the bonus is off the screen then reset the bonus else set its new position
			if(pos.y < -bonus->getContentSize().height/2 ) 
			{
				resetBonus();
			} 
			else 
			{
				bonus->setPosition(pos);
			}
		}
		
		// Update score based on how much the bird has moved
		score += (int)delta;

		// Display the new score value
		char scoreStr[10] = {0};
		sprintf(scoreStr, "%d", score);
		CCLabelBMFont* scoreLabel = (CCLabelBMFont*) getChildByTag(kScoreLabel);
		scoreLabel->setString(scoreStr);
	}

	// Set the birds position
	bird->setPosition(bird_pos);
}

/////////////////////////////////////////////////////////
// Update the player y velocity for jumping
// Base the y velocity on the x velocity
void GameScene::jump(void)
{
	bird_vel.y = 350.0f + fabsf(bird_vel.x);
}


void GameScene::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, kCCMenuTouchPriority + 1, true);
}

bool GameScene::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	return true;
}

/////////////////////////////////////////////////////////
// Touch on left side of screen moves player left
// Touch on right side of screen moves player right
void GameScene::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
	CCPoint touchLocation = touch->locationInView( touch->view() );	
	touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
	touchLocation =  convertToNodeSpace(touchLocation);

	float touchCenter =  CCDirector::sharedDirector()->getWinSize().width/2 - touchLocation.x;
	float accel_filter = 0.1f;
	bird_vel.x = bird_vel.x * accel_filter - touchCenter;  
}

/////////////////////////////////////////////////////////
// Acceleramoter routine to move the player object
void GameScene::didAccelerate(CCAcceleration* pAccelerationValue)
{
	if(gameSuspended) return;

	float accel_filter = 0.1f;
	bird_vel.x = bird_vel.x * accel_filter + pAccelerationValue->x * (1.0f - accel_filter) * 500.0f;

	// CCLog("GameScene::didAccelerate %f",bird_vel.x);
}
