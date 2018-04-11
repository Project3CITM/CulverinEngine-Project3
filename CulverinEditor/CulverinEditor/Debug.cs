using System.Runtime.CompilerServices;

namespace CulverinEditor.Debug
{
    public enum Color
    {
        NONE = -1,
        BLUE,
        GREEN,
        YELLOW,
        ORANGE,
        PINK,
        RED
    }

    public enum Department
    {
        GENERAL = 0,
        PLAYER,
        IA,
        STAGE
    }

    public class Debug
    {

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(object message);

        public static void Log(object message, Department department = Department.GENERAL, Color color = Color.NONE)
        {
            // ADD COLOR FILTER -----------------------------------
            switch (color)
            {
                case Color.NONE:
                    {
                        break;
                    }

                case Color.BLUE:
                    {
                        message += "[blue]";
                        break;
                    }

                case Color.GREEN:
                    {
                        message += "[green]";
                        break;
                    }

                case Color.YELLOW:
                    {
                        message += "[yellow]";
                        break;
                    }

                case Color.ORANGE:
                    {
                        message += "[orange]";
                        break;
                    }

                case Color.PINK:
                    {
                        message += "[pink]";
                        break;
                    }

                case Color.RED:
                    {
                        message += "[red]";
                        break;
                    }

                default:
                    break;
            }
            // -----------------------------------------------------

            // ADD DEPARTMENT FILTER --------------------------------
            switch (department)
            {
                case Department.GENERAL:
                    {
                        break;
                    }

                case Department.PLAYER:
                    {
                        message = "[Player] " + message;
                        break;
                    }

                case Department.IA:
                    {
                        message = "[IA] " + message;
                        break;
                    }

                case Department.STAGE:
                    {
                        message = "[Stage] " + message;
                        break;
                    }

                default:
                    break;
            }
            // ----------------------------------------------------

            //Call inEngine function
            Log(message);
        }
    }
}