using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ShowScore : CulverinBehaviour
{
    //Level tries
    public GameObject coin1_try;
    public GameObject coin2_try;
    public GameObject coin3_try;
    //Enemies alive
    public GameObject coin1_alive;
    public GameObject coin2_alive;
    public GameObject coin3_alive;
    //Enemies killed
    public GameObject coin1_kill;
    public GameObject coin2_kill;
    public GameObject coin3_kill;
    //Enemies killed
    public GameObject coin1_time;
    public GameObject coin2_time;
    public GameObject coin3_time;
    //TOTAL SCORE
    public GameObject bronze_coin;
    public GameObject silver_coin;
    public GameObject golden_coin;

    public int coin_score = 0;

    public int gold_score = 10; //one coin time
    public int silver_score = 5; //two coin time
    public int bronze_score = 3; //three coin time

    public int gold_value = 0; //one coin time
    public int silver_value = 0; //two coin time
    public int bronze_value = 0; //three coin time

    public int enemy_gold_kills = 2;
    public int enemy_silver_kills = 1;
    public int enemy_bronze_kills = 0;

    public int puzzle_gold_trys = 0;
    public int puzzle_silver_trys = 1;
    public int puzzle_bronze_trys = 2;

    public int characters_gold_alive = 3;
    public int characters_silver_alive = 2;
    public int characters_bronze_alive = 1;

    public float gold_time = 0.0f;
    public float silver_time = 1.0f;
    public float bronze_time = 2.0f;
    //space for sergio saez methods 
    void Start()
    {
       
        coin1_try = GetLinkedObject("coin1_try");
        coin2_try = GetLinkedObject("coin2_try");
        coin3_try = GetLinkedObject("coin3_try");
        coin1_alive = GetLinkedObject("coin1_alive");
        coin2_alive = GetLinkedObject("coin2_alive");
        coin3_alive = GetLinkedObject("coin3_alive");
        coin1_kill = GetLinkedObject("coin1_kill");
        coin2_kill = GetLinkedObject("coin2_kill");
        coin3_kill = GetLinkedObject("coin3_kill");
        coin1_time = GetLinkedObject("coin1_time");
        coin2_time = GetLinkedObject("coin2_time");
        coin3_time = GetLinkedObject("coin3_time");
        bronze_coin = GetLinkedObject("bronze_coin");
        silver_coin = GetLinkedObject("silver_coin");
        golden_coin = GetLinkedObject("golden_coin");
        bronze_coin.SetActive(false);
        silver_coin.SetActive(false);
        golden_coin.SetActive(false);

        coin1_kill.SetActive(false);
        coin2_kill.SetActive(false);
        coin3_kill.SetActive(false);
        coin1_alive.SetActive(false);
        coin2_alive.SetActive(false);
        coin3_alive.SetActive(false);
        coin1_try.SetActive(false);
        coin2_try.SetActive(false);
        coin3_try.SetActive(false);
        coin1_time.SetActive(false);
        coin2_time.SetActive(false);
        coin3_time.SetActive(false);

        EnemyKillsScore();
        AliveScore();
        TimeScore();
        TryScore();
        TotalScore();
    }
    void EnemyKillsScore()
    {
        if (StatsScore.enem_killed >= enemy_bronze_kills)
        {
            coin_score += bronze_value;
            coin1_kill.SetActive(true);
        }
        if (StatsScore.enem_killed >= enemy_silver_kills)
        {
            coin_score += silver_value;
            coin2_kill.SetActive(true);
        }
        if (StatsScore.enem_killed >= enemy_gold_kills)
        {
            coin_score += gold_value;
            coin3_kill.SetActive(true);
        }
    }
    void AliveScore()
    {
        if (StatsScore.characters_alive <= 3)
        {
            coin_score += bronze_value;
            coin1_alive.SetActive(true);

        }
        if (StatsScore.characters_alive <= 2)
        {
            coin_score += silver_value;
            coin2_alive.SetActive(true);
        }
        if (StatsScore.characters_alive <= 1)
        {
            coin_score += gold_value;
            coin3_alive.SetActive(true);

        }
    }
    void TimeScore()
    {
        if (StatsScore.time < bronze_time)
        {
            coin_score += bronze_value;
            coin1_time.SetActive(true);

        }
        if (StatsScore.characters_alive < silver_time)
        {
            coin_score += bronze_value;
            coin_score += silver_value;
            coin1_time.SetActive(true);
            coin2_time.SetActive(true);


        }
        if (StatsScore.characters_alive < gold_time)
        {
            coin_score += bronze_value;
            coin_score += silver_value;
            coin_score += gold_value;
            coin1_time.SetActive(true);
            coin2_time.SetActive(true);
            coin3_time.SetActive(true);

        }
    }
    void TryScore()
    {
        if (StatsScore.puzzle_tries <= puzzle_bronze_trys)
        {
            coin_score += bronze_value;
            coin1_try.SetActive(true);
        }
        if (StatsScore.puzzle_tries <= puzzle_silver_trys)
        {
            coin_score += bronze_value;
            coin_score += silver_value;
            coin1_try.SetActive(true);
            coin2_try.SetActive(true);
        }
        if (StatsScore.puzzle_tries <= puzzle_gold_trys)
        {
            coin_score += bronze_value;
            coin_score += silver_value;
            coin_score += gold_value;
            coin1_try.SetActive(true);
            coin2_try.SetActive(true);
            coin3_try.SetActive(true);
        }
    }

    void TotalScore()
    {    
        if(coin_score>=gold_score)
        {
            golden_coin.SetActive(true);

        }
        else if(coin_score >= silver_score)
        {
            silver_coin.SetActive(true);

        }
        else if (coin_score >= bronze_score)
        {
            bronze_coin.SetActive(true);

        }
    }
}
