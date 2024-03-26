//
// Created by 韩昱川 on 2/27/24.
//

#ifndef DUODYNO_CONTROLLER_H
#define DUODYNO_CONTROLLER_H

class Controller{
public:
	static Controller *getInstance();
	void beforeDraw();

private:
	int _pBodyCount=0;
};

#endif //DUODYNO_CONTROLLER_H
