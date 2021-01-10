//
// Created by lucas on 2020-12-25.
//

#pragma once

int main(int argc, char ** argv);

void FSGGameInitialise();
int FSGGameMain(double dElapsed);
int FSGGameDestroy();

bool FSGMainHasCommand(char const *szCommand, int argc, char **argv);

