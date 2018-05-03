using CulverinEditor;
using CulverinEditor.SceneManagement;


class DebugSceneChanger : CulverinBehaviour
{
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            SceneManager.LoadNewWalkableMap("Boss_Test_Map");
            SceneManager.LoadScene("ScoreScenev2");
        }
    }
}