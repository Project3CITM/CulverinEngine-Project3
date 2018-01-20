using System;

public class  Example {

	private String name;

	public Example(String name) {

 		this.name = name;
	}

	~Example() {

	}

	public String GetName() {

 		return name;
	}

	public void Process() {

 		throw new NotImplementedException("Not implemented yet");
	}

	public String Hello() {

 		return "Hello World";
	}

}
