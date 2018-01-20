using System;
using CulverinEditor;
using CulverinEditor.Debug;

namespace Example
{
    public class Example
    {
        private String name;

        public Example(String name)
        {
            this.name = name;
            System.Console.WriteLine("Entity " + name + " constructed");
        }

        ~Example()
        {
            System.Console.WriteLine("Entity " + name + " destructed");
        }

        public void Process()
        {
            throw new NotImplementedException("Not implemented yet");
        }

        public String GetName()
        {
            return name;
        }

        public String Hello()
        {
            return "Hello World";
        }
    }
}
