using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ShowScore2 : CulverinBehaviour
{

    public GameObject time_title;
    public GameObject time_title_gold;
    public GameObject survivors_title;
    public GameObject survivors_title_gold;
    public GameObject combat_title;
    public GameObject combat_title_gold;
    public GameObject skill_level_title;
    public GameObject skill_level_title_gold;
    public GameObject grade_title;
    public GameObject grade_title_gold;

    public GameObject time_text;
    public GameObject survivors_count;
    public GameObject combat_point;
    public GameObject skill_bronze_coin;
    public GameObject skill_silver_coin;
    public GameObject skill_gold_coin;
    public GameObject grade_mark_S;
    public GameObject grade_mark_A;
    public GameObject grade_mark_B;
    public GameObject grade_mark_C;
    public GameObject grade_mark_D;


    public int fountain_uses = 3;
    public int max_puzzle_hits = 5;
    public int boss_time_sec = 85;
    public float gold_time = 440;
    public float silver_time = 510;
    public int gold_combat_points = 480;
    public int silver_combat_points = 200;

    int grade_points = 0;
    int skill_coins = 0;

    void Start()
    {
        time_title = GetLinkedObject("time_title");
        time_title_gold = GetLinkedObject("time_title_gold");
        survivors_title = GetLinkedObject("survivors_title");
        survivors_title_gold = GetLinkedObject("survivors_title_gold");
        combat_title = GetLinkedObject("combat_title");
        combat_title_gold = GetLinkedObject("combat_title_gold");
        skill_level_title = GetLinkedObject("skill_level_title");
        skill_level_title_gold = GetLinkedObject("skill_level_title_gold");

        time_title_gold.SetActive(false);
        survivors_title_gold.SetActive(false);
        combat_title_gold.SetActive(false);
        skill_level_title_gold.SetActive(false);

        time_text = GetLinkedObject("time_text");
        survivors_count = GetLinkedObject("survivors_count");
        combat_point = GetLinkedObject("combat_point");
        skill_bronze_coin = GetLinkedObject("skill_bronze_coin");
        skill_silver_coin = GetLinkedObject("skill_silver_coin");
        skill_gold_coin = GetLinkedObject("skill_gold_coin");
        skill_bronze_coin.SetActive(false);
        skill_silver_coin.SetActive(false);
        skill_gold_coin.SetActive(false);

        grade_mark_S = GetLinkedObject("grade_mark_S");
        grade_mark_A = GetLinkedObject("grade_mark_A");
        grade_mark_B = GetLinkedObject("grade_mark_B");
        grade_mark_C = GetLinkedObject("grade_mark_C");
        grade_mark_D = GetLinkedObject("grade_mark_D");
        grade_mark_S.SetActive(false);
        grade_mark_A.SetActive(false);
        grade_mark_B.SetActive(false);
        grade_mark_C.SetActive(false);
        grade_mark_D.SetActive(false);

        grade_points = 0;
        skill_coins = 0;

        CalulateScore();
        DisplayScore();
    }

    void DisplayScore()
    {
        DisplayTime();
        DisplaySurvivors();
        DisplayCombatPoints();
        DisplaySkillLevel();
        DisplayGrade();
    }

    void DisplayTime()
    {
        StatsScore.GetTime();
        
        int minutes = (int)StatsScore.time/60;
        int seconds = (int)StatsScore.time%60;
 
        string text = minutes.ToString("D2") + ":" + seconds.ToString("D2");

        time_text.GetComponent<CompText>().SetText(text);

        if(StatsScore.time < gold_time)
        {
            time_title.SetActive(false);
            time_title_gold.SetActive(true);
        }

    }

    void DisplaySurvivors()
    {
        survivors_count.GetComponent<CompText>().SetText(StatsScore.characters_alive.ToString() + "/3");
        if(StatsScore.characters_alive == 3)
        {
            survivors_title.SetActive(false);
            survivors_title_gold.SetActive(true);
        }
    }

    void DisplayCombatPoints()
    {
        combat_point.GetComponent<CompText>().SetText(StatsScore.combat_points.ToString() + "/500");
        if(StatsScore.combat_points > gold_combat_points)
        {
            combat_title.SetActive(false);
            combat_title_gold.SetActive(true);
        }
    }

    void DisplaySkillLevel()
    {
        if (skill_coins >= 1)
        {
            skill_bronze_coin.SetActive(true);
        }
        if (skill_coins >= 2)
        {
            skill_silver_coin.SetActive(true);
        }
        if (skill_coins == 3)
        {
            skill_gold_coin.SetActive(true);
            skill_level_title.SetActive(false);
            skill_level_title_gold.SetActive(true);
        }
    }

    void DisplayGrade()
    {
        if(grade_points >= 400)
        {
            Debug.Log("Grade S", Department.GENERAL, Color.PINK);
            grade_mark_S.SetActive(true);
            grade_title.SetActive(false);
            grade_title_gold.SetActive(true);
        }
        else if (grade_points < 400 && grade_points >= 350)
        {
            Debug.Log("Grade A", Department.GENERAL, Color.PINK);
            grade_mark_A.SetActive(true);
        }
        else if (grade_points < 350 && grade_points >= 250)
        {
            Debug.Log("Grade B", Department.GENERAL, Color.PINK);
            grade_mark_B.SetActive(true);
        }
        else if (grade_points < 250 && grade_points >= 200)
        {
            Debug.Log("Grade C", Department.GENERAL, Color.PINK);
            grade_mark_C.SetActive(true);
        }
        else if (grade_points < 200)
        {
            Debug.Log("Grade D", Department.GENERAL, Color.PINK);
            grade_mark_D.SetActive(true);
        }
    }

    void CalulateScore()
    {
        if(StatsScore.fountain_times <= fountain_uses)
        {
            skill_coins++;
        }
        if(StatsScore.puzzle_hits <= max_puzzle_hits)
        {
            skill_coins++;
        }
        if(StatsScore.boss_time <= boss_time_sec)
        {
            skill_coins++;
        }


        //Grade--------------
        //Time
        if(StatsScore.time <= gold_time)
        {
            grade_points += 100;
        }
        else if (StatsScore.time > gold_time && StatsScore.time < silver_time)
        {
            grade_points += 50;
        }
        else if (StatsScore.time > silver_time)
        {
            grade_points += 25;
        }

        //Survivors
        if (StatsScore.characters_alive == 3)
        {
            grade_points += 100;
        }
        else if (StatsScore.characters_alive == 2)
        {
            grade_points += 50;
        }
        else if (StatsScore.characters_alive == 1)
        {
            grade_points += 25;
        }


        //Combat points
        if (StatsScore.combat_points > 480)
        {
            grade_points += 100;
        }
        else if (StatsScore.combat_points < 480 && StatsScore.combat_points > 200)
        {
            grade_points += 50;
        }
        else if (StatsScore.combat_points < 200)
        {
            grade_points += 25;
        }

        //Skill level
        if (skill_coins == 3)
        {
            grade_points += 100;
        }
        else if (skill_coins == 2)
        {
            grade_points += 50;
        }
        else if (skill_coins == 1)
        {
            grade_points += 25;
        }
    }
}