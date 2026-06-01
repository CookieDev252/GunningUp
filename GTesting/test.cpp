//allows testing
#include <gtest/gtest.h>

//grabs the values from GunningUp for testing
#include "GunningUp.h"



//----------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------Fixture Classes----------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//



struct TestPlayerAndWall : public ::testing::Test {
protected:
    void SetUp() override {
        t_wall = new Line2D({ -10,10 }, { 10,10 }, BLACK);
        t_player = new Player({ 0,0 }, { 0,0 });
    }

    void TearDown() override {
        delete t_wall;
        delete t_player;
    }
    Player* t_player;
    Line2D* t_wall;
};



struct TestBulletAndWall : public ::testing::Test {
protected:
    void SetUp() override {
        t_bullet = new Bullet();
        t_wall = new Line2D({ -10,10 }, { 10,10 }, BLACK);
    }

    void TearDown() override {
        delete t_bullet;
        delete t_wall;
    }
    Bullet* t_bullet;
    Line2D* t_wall;
};



struct TestEnemyAndPlayer : public ::testing::Test {
protected:
    void SetUp() override {
        t_player = new Player({}, {});
        t_enemy = new Enemy({}, {});
        t_enemy->setHurtSound("..\\assets\\audio\\e.mp3");
        t_enemy->setShootSound("..\\assets\\audio\\e.mp3");
        t_player->setHurtSound("..\\assets\\audio\\e.mp3");
        t_player->setShootSound("..\\assets\\audio\\e.mp3");
    }

    void TearDown() override {
        delete t_enemy;
        delete t_player;
    }
    Player* t_player;
    Enemy* t_enemy;

};


//----------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------TESTING SUITE-------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//

TEST_F(TestEnemyAndPlayer, ShouldChasePlayer) {
    EXPECT_TRUE(!t_enemy->isShootingPlayer());
}

TEST_F(TestEnemyAndPlayer, ShouldShootPlayer) {
    EXPECT_FALSE(t_enemy->isShootingPlayer());
}

TEST_F(TestEnemyAndPlayer, DamageEnemy) {
    t_enemy->Damage(t_player->getDamage());
    EXPECT_EQ(t_enemy->getHealth(), 90);
}
TEST_F(TestEnemyAndPlayer, IsEnemyHit) {
    t_enemy->Damage(t_player->getDamage());
    EXPECT_TRUE(t_enemy->isHit());
}

TEST_F(TestEnemyAndPlayer, EnemySetPlayerAndReturnPointer) {
    t_enemy->setPlayer(t_player);
    EXPECT_EQ(t_enemy->getPlayer(), t_player);
}
//TEST_F(TestEnemyAndPlayer, ) {
//
//}
//TEST_F(TestEnemyAndPlayer, ) {
//
//}
//TEST_F(TestEnemyAndPlayer, ) {
//
//}

//----------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
