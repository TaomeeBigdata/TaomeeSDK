package com.taomee.statlogger;

public enum TaskType {
	task_begin(0),task_newbie(1),task_story(2),task_supplement(3),task_end(4);
	private int value;
	private TaskType(){
		;
	}
	private TaskType(int value){
		this.value = value;
	}
	
	public int getValue(){
		return this.value;
	}
}
