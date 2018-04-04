using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ScoreScreen : CulverinBehaviour
{ 
    //For any method or data you need from player stats check and modify only if necessary StatsScore.cs

    //Time coins Time coins to link in inspector
    public GameObject coin1_time;
    public GameObject coin2_time;
    public GameObject coin3_time;
    public GameObject text_time;
    //Level tries to link in inspector
    public GameObject coin1_try;
    public GameObject coin2_try;
    public GameObject coin3_try;
    public GameObject text_try;
    //Enemies alive to link in inspector
    public GameObject coin1_alive;
    public GameObject coin2_alive;
    public GameObject coin3_alive;
    public GameObject text_alive;
    //Enemies killed to link in inspector
    public GameObject coin1_kill;
    public GameObject coin2_kill;
    public GameObject coin3_kill;
    public GameObject text_kill;
    //TOTAL SCORE to link in inspector
    public GameObject bronze_coin;
    public GameObject silver_coin;
    public GameObject golden_coin;
    //texts for total score to link in inspector
    public GameObject bronze;
    public GameObject silver;
    public GameObject golden;
    public int coin_count = 0;
    //values to balance score
    //time
    public float oneCoin_time_lvl = 0;
    public float twoCoin_time_lvl = 0; 
    public float threeCoin_time_lvl = 0;
    //tries
    public float oneCoin_try_lvl = 0; 
    public float twoCoin_try_lvl = 0; 
    public float threeCoin_try_lvl = 0;
    //enemies alive
    public float oneCoin_alive_lvl = 0; 
    public float twoCoin_alive_lvl = 0; 
    public float threeCoin_alive_lvl = 0;
    //enemy killed
    public float oneCoin_kill_lvl = 0;
    public float twoCoin_kill_lvl = 0;
    public float threeCoin_kill_lvl = 0;
    //Final coin
    public int TOTAL_COINS = 0;
    public int goldf_coin = 0;
    public int silverf_coin = 0;
    public int bronzef_coin = 0;

    public void TimeCoins()
    {
        float time_lvl = Time.timePlay;
        if(time_lvl <= threeCoin_time_lvl)
        {
            coin3_time.SetActive(true);
            TOTAL_COINS++;
        }
        if(time_lvl <= twoCoin_time_lvl)
        {
            coin2_time.SetActive(true);
            TOTAL_COINS++;
        }
        if (time_lvl <= oneCoin_time_lvl)
        {
            coin1_time.SetActive(true);
            TOTAL_COINS++;
        }
        text_time.SetActive(true);  //this is for text
    }

    public void TryCoins()
    {
        float tries_lvl = Data.puzzle_tries;
        if (tries_lvl <= threeCoin_try_lvl)
        {
            coin3_try.SetActive(true);
            TOTAL_COINS++;
        }
        if (tries_lvl <= twoCoin_try_lvl)
        {
            coin2_try.SetActive(true);
            TOTAL_COINS++;
        }
        if (tries_lvl <= oneCoin_try_lvl)
        {
            coin1_try.SetActive(true);
            TOTAL_COINS++;
        }
        text_try.SetActive(true);  //this is for text
    }

    public void EnemAliveCoins()
    {
        float enem_alive = Data.total_enemies - Data.enem_killed;
        if (enem_alive <= threeCoin_alive_lvl)
        {
            coin3_alive.SetActive(true);
            TOTAL_COINS++;
        }
        if (enem_alive <= twoCoin_alive_lvl)
        {
            coin2_alive.SetActive(true);
            TOTAL_COINS++;
        }
        if (enem_alive <= oneCoin_alive_lvl)
        {
            coin1_alive.SetActive(true);
            TOTAL_COINS++;
        }
        text_alive.SetActive(true);  //this is for text
    }

    public void KillsCoins()
    {
        float total_kills = Data.enem_killed;
        if (total_kills <= threeCoin_kill_lvl)
        {
            coin3_kill.SetActive(true);
            TOTAL_COINS++;
        }
        if (total_kills <= twoCoin_kill_lvl)
        {
            coin2_kill.SetActive(true);
            TOTAL_COINS++;
        }
        if (total_kills <= oneCoin_kill_lvl)
        {
            coin1_kill.SetActive(true);
            TOTAL_COINS++;
        }
        text_kill.SetActive(true);  //this is for text
    }

    public void FinalCoin()
    {
        if(TOTAL_COINS > silverf_coin)
        {
            bronze_coin.SetActive(true);
        }
        if(TOTAL_COINS >= bronzef_coin && TOTAL_COINS <= silverf_coin)
        {
            silver_coin.SetActive(true);
        }
        if (TOTAL_COINS > silverf_coin || TOTAL_COINS == goldf_coin)
        {
            golden_coin.SetActive(true);
        }      
    }
}
