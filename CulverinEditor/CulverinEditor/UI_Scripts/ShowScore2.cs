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

        DisplayScore();
    }

    void DisplayScore()
    {
        DisplayTime();
        DisplaySurvivors();
      //  DisplayCombatPoints();
      //  DisplaySkillLevel();
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

    }

    void DisplaySkillLevel()
    {
        if (StatsScore.characters_alive >= 1)
        {
            skill_bronze_coin.SetActive(true);
        }
        if (StatsScore.characters_alive >= 2)
        {
            skill_silver_coin.SetActive(true);
        }
        if (StatsScore.characters_alive == 3)
        {
            skill_gold_coin.SetActive(true);
        }
    }
}