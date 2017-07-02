#include "ester-config.h"
#include "utils.h"
#include "star.h"
#include "read_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[]) {

	int nit,last_it;
	double err;
	tiempo t;
	double t_plot;
	configuration config(argc,argv);
	figure *fig = NULL;
	
	t.start();
	if(config.verbose) {
		fig=new figure(config.plot_device);
	//	fig->subplot(2,1);
	}
	
	star1d A;
	solver *op;
	
	if(A.init(config.input_file,config.param_file,argc,argv)) {
        ester_err("Could not initialize star");
        return 1;
    }

	
	matrix tt(config.maxit+1,1),error(config.maxit+1,1);

	t_plot=0;
	//last_it=nit>=config.maxit; // last_it=0 normally
	op=A.init_solver();
	if(config.verbose>2) op->verbose=1;
	A.config.newton_dmax=config.newton_dmax;
	if(config.verbose>1) A.config.verbose=1;
	
	// If no input file, ignore core convection until the model starts to converge
	int core_convec_set=A.core_convec;
	int env_convec_set=A.env_convec;
	if(*config.input_file==0) {
		A.core_convec=0;
		A.env_convec=0;
	}
	SDIRK_solver rk;
        rk.init(3, "sdirk3");
        rk.regvar("X", A.Xh);
        rk.regvar("rho", A.rho);
        rk.regvar("log_rhoc", log(A.rhoc)*ones(1,1));

        rk.set_step(A.dtime);

        int state;
        while((state = rk.solve(0.,20.)) != RK_END) {
                A.delta = rk.get_delta();
                A.time = rk.get_t();
                A.Xh0 = rk.get_var("X");
                A.rho0 = rk.get_var("rho");
                A.rhoc0 = exp(rk.get_var("log_rhoc")(0));

        last_it=0;
	err=1;
	nit=0;
	while(!last_it) {
		if(err<0.1&&!*config.input_file) {
			A.core_convec=core_convec_set;
			A.env_convec=env_convec_set;
		}
		nit++;
		//A.check_jacobian(op,"log_T");exit(0);
		err=A.solve(op);
		
		tt(nit-1)=t.value();
		error(nit-1)=err;
		last_it=(err<config.tol&&nit>=config.minit)||nit>=config.maxit;
		if(config.verbose) {
			printf("it=%d err=%e\n",nit,err);
			
	//		if(tt(nit-1)-t_plot>config.plot_interval||last_it) {
	//			fig->semilogy(error.block(0,nit-1,0,0));
	//			fig->label("Iteration number","Relative error","");
	//			A.spectrum(fig,A.rho);
	//			fig->label("Density (normalized spectrum)","","");
	//			t_plot=tt(nit-1);
			//}

		}

	}
                rk.set_var("X",A.Xh);
                rk.set_var("rho",A.rho);
                rk.set_var("log_rhoc",log(A.rhoc)*ones(1,1));
                if(state == RK_STEP) {
			//fig->axis(0.,1.,0.6,0.71);
                        fig->plot(A.r, A.Vr);
                        fig->hold(1);
                        printf("t = %f  \n", A.time );
                }
        }

	if(config.verbose) {
		printf("Mass=%3.3f Msun  Radius=%3.3f Rsun  Luminosity=%3.3f Lsun  Teff=%1.1f K\n",
				A.M/M_SUN,A.R/R_SUN,A.luminosity()/L_SUN,A.Teff()(0));
		printf("X=%3.3f (Xc/X=%3.3f) Z=%3.3f\n",A.X0,A.Xc,A.Z0);
		printf("rhoc=%e Tc=%e pc=%e\n",A.rhoc,A.Tc,A.pc);
		if(A.conv) printf("r_cz=%3.3f Rsun\n",*(A.map.gl.xif+A.conv)*A.R/R_SUN);
	}
	delete op;
	A.write(config.output_file,config.output_mode);
	
	if(config.verbose) {
		delete fig;
	}
	
	t.stop();
	if(config.verbose) 
		printf("%2.2f seconds\n",t.value());	
	return 0;
}
