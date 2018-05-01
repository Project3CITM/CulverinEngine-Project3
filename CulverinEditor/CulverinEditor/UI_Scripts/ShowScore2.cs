using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ShowScore2 : CulverinBehaviour
{
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


    int grade_points = 0;
    int skill_coins = 0;

    void Start()
    {
        time_text = GetLinkedObject("time_text");
        survivors_count = GetLinkedObject("survivors_count");
        combat_point = GetLinkedObject("combat_point");
        skill_bronze_coin = GetLinkedObject("skill_bronze_coin");
        skill_silver_coin = GetLinkedObject("skill_silver_coin");
        skill_gold_coin = GetLinkedObject("skill_gold_coin");
        skill_bronze_coin.SetActive(false);
        skill_silver_coin.SetActive(false);
        skill_gold_coin.SetActive(false);
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
    }

    void DisplaySurvivors()
    {
        survivors_count.GetComponent<CompText>().SetText(StatsScore.characters_alive.ToString() + "/3");
    }

    void DisplayCombatPoints()
    {
        combat_point.GetComponent<CompText>().SetText(StatsScore.combat_points.ToString() + "/500");
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
        }
    }

    void DisplayGrade()
    {
        if(grade_points >= 400)
        {
            grade_mark_S.SetActive(true);
        }
        else if (grade_points < 400 && grade_points >= 350)
        {
            grade_mark_A.SetActive(true);
        }
        else if (grade_points < 350 && grade_points >= 250)
        {
            grade_mark_B.SetActive(true);
        }
        else if (grade_points < 250 && grade_points >= 200)
        {
            grade_mark_C.SetActive(true);
        }
        else if (grade_points < 200)
        {
            grade_mark_D.SetActive(true);
        }
    }

    void CalulateScore()
    {
        if(StatsScore.fountain_times <= 3)
        {
            skill_coins++;
        }
        if(StatsScore.puzzle_hits <= 5)
        {
            skill_coins++;
        }


        //Grade--------------
        //Time
        if(StatsScore.time <= 440)
        {
            grade_points += 100;
        }
        else if (StatsScore.time > 440 && StatsScore.time < 510)
        {
            grade_points += 50;
        }
        else if (StatsScore.time > 510)
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