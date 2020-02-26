#include "Player.hh"
#include <climits>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Maal
#define ocupat -2

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

 typedef vector<int> VI;
 vector<pair <int,int> > vp = {{1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1}};


vector<vector <int>> incilitazarTauler() {




    vector<vector <int>> M(60, vector<int>(60, -1));
	for(int u : dwarves(me())) M[unit(u).pos.i][unit(u).pos.j] = ocupat;
	for(int u : wizards(me())) M[unit(u).pos.i][unit(u).pos.j] = ocupat;
    	Pos p = unit(balrog_id()).pos;

      	for(int u : trolls()) M[unit(u).pos.i][unit(u).pos.j] = ocupat;


	M[p.i][ p.j] = ocupat;
    for (pair<int, int> w : vp)
        M[p.i + w.first][ p.j + w.second] = -3;

    return M;

}

//Busca enemics jugadors
bool findEnemyDwarves(int id) {

	for(int i=0; i<4; i++) 
            if(i != me()) {
		for(int u : dwarves(i))
			if(id==u)return true;
		for(int u: wizards(i))
			if(id==u) return true;	
		}
               
	return false;
	
}

//Busca enemics Moria
bool enemies(int id) {

	if(id == -1) return false;

	for(int u : trolls()) 
            if(u == id) return true;
        
	for(int u : orcs()) 
            if(u == id) return true;
       
	return false;
	
}

bool findOrcs(int id) {

    for(int u : orcs()) if(u == id) return true;

    return false;


}

//Caselles a conquerir

    pair<int, Pos> bfs_pintar (Pos p, vector<vector <int>> M) {

        vector<pair <int,int> > vp = {{1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1}};
        queue<Pos> q;
        vector<vector <Pos> > rec(60, vector<Pos>(60));
        q.push(p);
        M[p.i][p.j] = 0;

        while (not q.empty()) {
            Pos v = q.front();
            q.pop();

            for (pair<int, int> w : vp) {

                Pos pos(v.i + w.first, v.j + w.second);

                if (pos_ok(pos) and M[pos.i][pos.j] == -1) {
                    Cell c =cell(pos);

                    if(c.type == Cave or c.type == Outside) {

                        rec[pos.i][pos.j] = v;
                        M[pos.i][pos.j] = M[v.i][v.j] + 1;

                        if(c.owner != me() and not c.type == Outside)
                            return make_pair(M[pos.i][pos.j], dec(rec, pos, p));

                        if (cell(pos).id != balrog_id()) q.push(pos);

                    }
                }
            }
        }
        return {0,p};
    }


 pair<int, Pos> bfs_pintar_Rocks (Pos p, vector<vector <int>> M) {

        vector<pair <int,int> > vp = {{1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1}};
        queue<Pos> q;
        vector<vector <Pos> > rec(60, vector<Pos>(60));
        q.push(p);
        M[p.i][p.j] = 0;

        while (not q.empty()) {
            Pos v = q.front();
            q.pop();

            for (pair<int, int> w : vp) {

                Pos pos(v.i + w.first, v.j + w.second);

                if (pos_ok(pos) and M[pos.i][pos.j] == -1) {
                    Cell c =cell(pos);

                    if(c.type == Cave or c.type == Rock  or c.type == Outside) {

                        rec[pos.i][pos.j] = v;
                        M[pos.i][pos.j] = (c.type == Rock) ? c.turns + M[v.i][v.j] :  M[v.i][v.j] + 1;

                        if(c.type == Rock)
                            return make_pair(M[pos.i][pos.j], dec(rec, pos, p));

                        q.push(pos);

                    }
                }
            }
        }
        return {0,p};
    }

 
	
//Buscar rival mes proper

pair<int, Pos> bfs_enemies (Pos p, vector<vector <int>> M) {
	
	vector<pair <int,int> > vp = {{1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1}};
	queue<Pos> q;
	vector<vector <Pos> > rec(60, vector<Pos>(60));
	q.push(p);
	M[p.i][p.j] = 0;
	
	while (not q.empty()) {
		Pos v = q.front();
		q.pop();

		for (pair<int, int> w : vp) {

	              Pos pos(v.i + w.first, v.j + w.second);

	              if (pos_ok(pos) and M[pos.i][pos.j] == -1) {
				Cell c =cell(pos);

				if(c.type == Cave or c.type == Outside) {
                  	
					rec[pos.i][pos.j] = v;
			          	M[pos.i][pos.j] = M[v.i][v.j] + 1;

                    if(c.id != -1) {
                        if (findEnemyDwarves(c.id))
                            return make_pair(M[pos.i][pos.j], dec(rec, pos, p));

                    }


                        if (cell(pos).id != balrog_id()) q.push(pos);

		    		}
              }
        }
	}
	return {0,p};
  }


 pair<int, Pos> bfs_treasures(Pos p, vector<vector <int>> M) {
	
	vector<pair <int,int> > vp = {{1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1}};
	queue<Pos> q;
	vector<vector <Pos> > rec(60, vector<Pos>(60));
	q.push(p);
	M[p.i][p.j] = 0;
	
	while (not q.empty()) {
		Pos v = q.front();
		q.pop();

		for (pair<int, int> w : vp) {

	            Pos pos(v.i + w.first, v.j + w.second);

             	    if (pos_ok(pos) and M[pos.i][pos.j] == -1 and cell(pos).type != Granite and cell(pos).type != Abyss) {
	                  
			        rec[pos.i][pos.j] = v;
        	        M[pos.i][pos.j] = M[v.i][v.j] + 1;

        	        if (cell(pos).treasure) 
				        return make_pair(M[pos.i][pos.j],dec(rec, pos, p));

                        if (cell(pos).id != balrog_id()) q.push(pos);

		}
            }
	}
	return {0,p};
  }



//Mous i actualitzar Matriu
void moveC(int id, Pos a, Pos d, vector<vector <int>>& M) {
    
	if(cell(d).id == -1) {
		M[a.i][a.j] = -1;
		M[d.i][d.j] = ocupat;
	}
    int x = a.i - d.i;
    int y = a.j - d.j;	
    if(x > 0 and y == 0) command(id, Top);
    if(x > 0 and y > 0) command(id, TL);
    if(x == 0 and y < 0) command(id, Right);
    if(x < 0 and y < 0) command(id, BR);
    if(x < 0 and y == 0) command(id, Bottom);
    if(x < 0 and y > 0) command(id, LB);
    if(x == 0 and y > 0) command(id, Left);
    if(x > 0 and y < 0) command(id, RT);

}
//Saber la dirreccio de per anar
Pos dec(vector<vector <Pos>>& rec, Pos fi, Pos ini) {	
	Pos f;
	for(f = fi; f != ini;) {
		if(rec[f.i][f.j] == ini) return f; 
		f = rec[f.i][f.j];	
	}

	return ini;

  }
//funció principal moure dwarves
 void move_dwarves(vector<vector <int>>& M) {

    VI D = dwarves(me());
    int n = D.size();
    for(int i = 0; i < n; ++i) {
		int id = D[i];
		Pos a = unit(id).pos;
        	
	if(n<5) {
       		        pair<int, Pos> t = bfs_treasures(a,M);
			pair<int, Pos> e = bfs_enemies(a,M);
		
		if(e.first < 3) moveC(id, a, e.second, M);
		else  moveC(id, a, t.second, M);

	} else {

		pair<int, Pos> t = bfs_treasures(a,M);
		pair<int, Pos> e = bfs_enemies(a, M);
		pair<int, Pos> c = bfs_pintar(a, M);
		pair<int, Pos> cp = bfs_pintar_Rocks(a, M);



       if(round() < 125) {
	     if (t.first < e.first) moveC(id, a, t.second, M);
            else moveC(id, a, e.second, M);
        } else {
		
             if(c.first < e.first and c.first < t.first and c.first < cp.first) moveC(id,a,c.second,M);
		else if(cp.first < e.first and cp.first < t.first) moveC(id,a,cp.second,M);
            else if(e.first < t.first) moveC(id, a, e.second, M);
            else moveC(id, a, t.second, M);
	}

	}
	}
  }


  //FUNCIONS WIZARDS

    //Busca els meus dwarves

    bool findMyDwarves(int id) {

        if(id == -1 ) return false;

        for(int u : dwarves(me()))
                    if(id==u)return true;

        return false;

    }

//Buscar enano mes proper
    Pos bfs_Wiz (Pos p, vector<vector <int>> M) {

        vector<pair <int,int> > vp = {{1,0},  {0,-1}, {-1,0}, {0,1}};
        queue<Pos> q;
        vector<vector <Pos> > rec(60, vector<Pos>(60));
        q.push(p);
        M[p.i][p.j] = 0;

        while (not q.empty()) {
            Pos v = q.front();
            q.pop();

            for (pair<int, int> w : vp) {

                Pos pos(v.i + w.first, v.j + w.second);

                if (pos_ok(pos) and M[pos.i][pos.j] == -1) {
                    Cell c =cell(pos);

                    if(c.type == Cave or c.type == Outside) {

                        rec[pos.i][pos.j] = v;
                        M[pos.i][pos.j] = M[v.i][v.j] + 1;

                        if(findMyDwarves(c.id))
                            return dec(rec, pos, p);

                        q.push(pos);

                    }
                }
            }
        }
        return p;
    }


//busca si te un enano tocant
    bool notInrange(Pos p) {

        vector<pair <int,int> > vp = {{1,0},  {0,-1}, {-1,0}, {0,1}};

        for (pair<int, int> w : vp) {

            Pos pos(p.i + w.first, p.j + w.second);

            if (pos_ok(pos) and findMyDwarves(cell(pos).id)) return true;

        }

        return false;

    }

//moure wizars falta millorar per fugir denemics
    void moveW(int id, Pos inici, Pos f, vector<vector <int>>& M) {
        int x = inici.i - f.i;
        int y = inici.j - f.j;

        if(x > 0 and y == 0) {
            	if(cell(f).id == -1 and mirar_top(f, M)) command(id, Top);
		else if (check(inici+Right) and mirar_right(inici+Right, M)) command(id, Right);
		else if (check(inici+Bottom) and mirar_south(inici+Bottom, M)) command(id, Bottom);	
		else if (check(inici+Left) and mirar_left(inici+Left, M)) command(id, Left);	
        } else if(x < 0 and y == 0) {
               	if(cell(f).id == -1 and mirar_south(f, M)) command(id, Bottom);
		else if (check(inici+Right) and mirar_right(inici+Right, M)) command(id, Right);
		else if (check(inici+Left) and mirar_left(inici+Left, M)) command(id, Left);	
		else if (check(inici+Top) and mirar_top(inici+Top, M)) command(id, Top);		 
        } else if(x == 0 and y < 0) {
           	if (cell(f).id == -1 and mirar_right(f, M)) command(id, Right);
		else if (check(inici+Top) and mirar_top(inici+Top, M)) command(id, Top);		 
		else if (check(inici+Bottom) and mirar_south(inici+Bottom, M)) command(id, Bottom);	
		else if (check(inici+Left) and mirar_left(inici+Left, M)) command(id, Left);		

        } else if(x == 0 and y > 0) {
            if (cell(f).id == -1 and mirar_left(f, M)) command(id, Left);
            else if (check(inici+Bottom) and mirar_south(inici+Bottom, M)) command(id, Bottom);	
            else if (check(inici+Top) and mirar_top(inici+Top, M)) command(id, Top);		 
		else if (check(inici+Right) and mirar_right(inici+Right, M)) command(id, Right);
}


    }


bool check(Pos p) {
	if(pos_ok(p) and cell(p).id == -1 and (cell(p).type == Cave or cell(p).type == Outside)) return true;

return false;

}

bool mirar_top(Pos p, vector<vector <int>>& M) {

	Pos o(p.i-1,p.j-1);

	for(int i=0; i<2; i++)
	    for(int j=0; j<3; j++) {
		Pos pos(o.i+i,o.j+j);
		if(pos_ok(pos.i,pos.j) and (cell(pos).id ==  balrog_id())) return false;
	}
	

	return true;

}

bool mirar_south(Pos p, vector<vector <int>>& M) {

	Pos o(p.i,p.j-1);

	for(int i=0; i<2; i++)
	    for(int j=0; j<3; j++) {
		Pos pos(o.i+i,o.j+j);
		if(pos_ok(pos.i,pos.j) and (cell(pos).id ==  balrog_id())) return false;
	}
	

	return true;

}

bool mirar_left(Pos p, vector<vector <int>>& M) {

	Pos o(p.i-1,p.j-1);

	for(int i=0; i<2; i++)
	    for(int j=0; j<3; j++) {
		Pos pos(o.i+j,o.j+i);
		if(pos_ok(pos.i,pos.j) and (cell(pos).id ==  balrog_id())) return false;
	}	



	return true;

}

bool mirar_right(Pos p, vector<vector <int>>& M) {

	Pos o(p.i-1,p.j);

	for(int i=0; i<2; i++)
	    for(int j=0; j<3; j++) {
		Pos pos(o.i+j,o.j+i);
		if(pos_ok(pos.i,pos.j) and (cell(pos).id ==  balrog_id())) return false;
	}	



	return true;

}




void move_wizards(vector<vector <int>>& M) {

    VI D = wizards(me());
    int n = D.size();
    for(int i = 0; i < n; ++i) {
        int id = D[i];
        Pos a = unit(id).pos;

        if(not notInrange(a)) {

            //falta comprovar si n esta en perill

            Pos p = bfs_Wiz(a, M);
            moveW(id, a, p, M);
        }


    }


}


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

      vector<vector <int>> M = incilitazarTauler();
      move_dwarves(M);
      move_wizards(M);

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
