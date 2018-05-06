using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ShowScore2 : CulverinBehaviour
{
    public GameObject time_text;
    public GameObject survivors_count;
    public GameObject combat_point;
    public GameObject skill_display;
    public GameObject grade_mark_S;
    public GameObject grade_mark_A;
    public GameObject grade_mark_B;
    public GameObject grade_mark_C;
    public GameObject grade_mark_D;

    public int max_combat_points = 500;
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
        time_text = GetLinkedObject("time_text");
        survivors_count = GetLinkedObject("survivors_count");
        combat_point = GetLinkedObject("combat_point");
        skill_display = GetLinkedObject("skill_display");

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
            time_text.GetComponent<CompText>().SetColor(new Vector3(1.0f, 1.0f, 0.0f), 1.0f);
        }

    }

    void DisplaySurvivors()
    {
        survivors_count.GetComponent<CompText>().SetText(StatsScore.characters_alive.ToString() + "/3");
        if(StatsScore.characters_alive == 3)
        {
            survivors_count.GetComponent<CompText>().SetColor(new Vector3(1.0f, 1.0f, 0.0f), 1.0f);
        }
    }

    void DisplayCombatPoints()
    {
        combat_point.GetComponent<CompText>().SetText(StatsScore.combat_points.ToString() + "/" + max_combat_points);
        if(StatsScore.combat_points > gold_combat_points)
        {
            combat_point.GetComponent<CompText>().SetColor(new Vector3(1.0f, 1.0f, 0.0f), 1.0f);
        }
    }

    void DisplaySkillLevel()
    {
    
        skill_display.GetComponent<CompText>().SetText(skill_coins.ToString() + "/3");
      
        if (skill_coins == 3)
        {
            skill_display.GetComponent<CompText>().SetColor(new Vector3(1.0f, 1.0f, 0.0f), 1.0f);
        }
    }

    void DisplayGrade()
    {
        if(grade_points >= 400)
        {
            Debug.Log("Grade S", Department.GENERAL, Color.PINK);
            grade_mark_S.SetActive(true);
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