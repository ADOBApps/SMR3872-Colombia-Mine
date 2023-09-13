/*
 * simple lennard-jones potential MD code with velocity verlet.
 * units: Length=Angstrom, Mass=amu; Energy=kcal
 *
 * baseline c version.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

/* generic file- or pathname buffer length */
#define BLEN 200

/* a few physical constants */
const double kboltz=0.0019872067;     /* boltzman constant in kcal/mol/K */
const double mvsq2e=2390.05736153349; /* m*v^2 in kcal/mol */

/* structure to hold the complete information
 * about the MD system */
struct _mdsys {
    int natoms, nfi, nsteps;
    double dt, mass, epsilon, sigma, box, rcut;
    double ekin, epot, temp;
    double *rx, *ry, *rz;
    double *vx, *vy, *vz;
    double *fx, *fy, *fz;
    double *cx, *cy, *cz; // new
    MPI_Comm mpicomm; // new
    int mpisize; // new (num_proceses)
    int mpirank;
};
typedef struct _mdsys mdsys_t;

/* helper function: read a line and then return
   the first string with whitespace stripped off */
static int get_a_line(FILE *fp, char *buf){
    char tmp[BLEN], *ptr;

    /* read a line and cut of comments and blanks */
    if (fgets(tmp,BLEN,fp)) {
        int i;
        ptr=strchr(tmp,'#');
        if (ptr) *ptr= '\0';
        i=strlen(tmp); --i;
        while(isspace(tmp[i])) {
            tmp[i]='\0';
            --i;
        }
        ptr=tmp;
        while(isspace(*ptr)) {++ptr;}
        i=strlen(ptr);
        strcpy(buf,tmp);
        return 0;
    } else {
        perror("problem reading input");
        return -1;
    }
    return 0;
}

/* helper function: get current time in seconds since epoch */

static double wallclock(){
        struct timeval t;
        gettimeofday(&t,0);
        return ((double) t.tv_sec) + 1.0e-6*((double) t.tv_usec);
}

/* helper function: zero out an array */
static void azzero(double *d, const int n){
    int i;
    for (i=0; i<n; ++i) {
        d[i]=0.0;
    }
}

/* helper function: apply minimum image convention */
static double pbc(double x, const double boxby2){
    while (x >  boxby2) x -= 2.0*boxby2;
    while (x < -boxby2) x += 2.0*boxby2;
    return x;
}

/* compute kinetic energy */
static void ekin(mdsys_t *sys){
    int i;
    sys->ekin = 0.0;
    for (i=0; i<sys->natoms; ++i) {
        sys->ekin += 0.5*mvsq2e*sys->mass*(sys->vx[i]*sys->vx[i] + sys->vy[i]*sys->vy[i] + sys->vz[i]*sys->vz[i]);
    }
    sys->temp = 2.0*sys->ekin/(3.0*sys->natoms-3.0)/kboltz;
}

/* compute forces */
static void force(mdsys_t *sys){
    double r,ffac;
    double rx, ry, rz;
    double rcsq, rsq;
    int i, ii, j;
    int c12, c6;

    /* zero energy and forces */
    // sys->epot=0.0;
    double epot = 0.0;// new
    /* 
    * Old
    azzero(sys->fx, sys->natoms);
    azzero(sys->fy, sys->natoms);
    azzero(sys->fz, sys->natoms); */
    azzero(sys->cx, sys->natoms);
    azzero(sys->cy, sys->natoms);
    azzero(sys->cz, sys->natoms);

    // Initialises the MPI environment
    MPI_Init(NULL, NULL);

    // Get current process id
    MPI_Comm_rank(MPI_COMM_WORLD, &sys->mpirank);

    // Get total proc
    MPI_Comm_size(MPI_COMM_WORLD, &sys->mpisize);


    /**
     * int MPI_Bcast(void* buffer,
              int count,
              MPI_Datatype datatype,
              int emitter_rank,
              MPI_Comm communicator);
    */

    MPI_Bcast(sys->cx, sys->natoms, MPI_DOUBLE, 0, sys->mpicomm); // new
    MPI_Bcast(sys->cy, sys->natoms, MPI_DOUBLE, 0, sys->mpicomm); // new
    MPI_Bcast(sys->cz, sys->natoms, MPI_DOUBLE, 0, sys->mpicomm); // new

    c12 = 4.0 * sys->epsilon * pow(sys->sigma, 12.0); // new
    c6 = 4.0 * sys->epsilon * pow(sys->sigma, 6.0); // new
    rcsq = sys->rcut * sys->rcut; // new
    for(i = 0; i < (sys->natoms) - 1; i = sys->mpisize) {
        ii = i + sys->mpirank;
        if(ii >= (sys->natoms - 1)){
            break;
        }
        for(j = (i +1); j < (sys->natoms); ++j) {
            /* particles have no interactions with themselves */
            /* if (i == j){
                continue;
            } */

            /* get distance between particle i and j */
            rx = pbc(sys->rx[i] - sys->rx[j], 0.5*sys->box);
            ry = pbc(sys->ry[i] - sys->ry[j], 0.5*sys->box);
            rz = pbc(sys->rz[i] - sys->rz[j], 0.5*sys->box);
            r = sqrt(rx*rx + ry*ry + rz*rz);
            rsq = rx*rx + ry*ry + rz*rz; // new

            if(rsq < rcsq){ // new
                double r6, r_inv; // new
                r_inv = 1.0/rsq; // new
                r6 = r_inv * r_inv * r_inv; // new
                ffac = ((2.0 * c12 * r6) - c6) * 6.0 * r6 * r_inv * r; // new
                sys->epot += (r6 * ((c12 * r6) - c6)); // new
                sys->fx[i] += rx/r*ffac;
                sys->fy[i] += ry/r*ffac;
                sys->fz[i] += rz/r*ffac;
                sys->fx[j] -= rx/r*ffac; // new
                sys->fy[j] -= ry/r*ffac; // new
                sys->fz[j] -= rz/r*ffac; // new
                sys->cy[i] -= ry * ffac;
                sys->cz[i] -= rz * ffac;
            }
        }
    }
    /*
    * MPI_Reduce(const void* send_buffer,
               void* receive_buffer,
               int count,
               MPI_Datatype datatype,
               MPI_Op operation,
               int root,
               MPI_Comm communicator);
    */
    MPI_Reduce(sys->cx, sys->fx, sys->natoms, MPI_DOUBLE, MPI_SUM, 0, sys->mpicomm);
    MPI_Reduce(sys->cy, sys->fy, sys->natoms, MPI_DOUBLE, MPI_SUM, 0, sys->mpicomm);
    MPI_Reduce(sys->cz, sys->fz, sys->natoms, MPI_DOUBLE, MPI_SUM, 0, sys->mpicomm);
    MPI_Reduce(&epot, &sys->epot, 1, MPI_DOUBLE, MPI_SUM, 0, sys->mpicomm);
    // finish cycle
    MPI_Finalize();
}

/* velocity verlet */
static void velverlet(mdsys_t *sys){
    int i;

    /* first part: propagate velocities by half and positions by full step */
    for (i=0; i<sys->natoms; ++i) {
        sys->vx[i] += 0.5*sys->dt / mvsq2e * sys->fx[i] / sys->mass;
        sys->vy[i] += 0.5*sys->dt / mvsq2e * sys->fy[i] / sys->mass;
        sys->vz[i] += 0.5*sys->dt / mvsq2e * sys->fz[i] / sys->mass;
        sys->rx[i] += sys->dt*sys->vx[i];
        sys->ry[i] += sys->dt*sys->vy[i];
        sys->rz[i] += sys->dt*sys->vz[i];
    }

    /* compute forces and potential energy */
    force(sys);

    /* second part: propagate velocities by another half step */
    for (i=0; i<sys->natoms; ++i) {
        sys->vx[i] += 0.5*sys->dt / mvsq2e * sys->fx[i] / sys->mass;
        sys->vy[i] += 0.5*sys->dt / mvsq2e * sys->fy[i] / sys->mass;
        sys->vz[i] += 0.5*sys->dt / mvsq2e * sys->fz[i] / sys->mass;
    }
}

/* append data to output. */
static void output(mdsys_t *sys, FILE *erg, FILE *traj){
    int i;

    printf("% 8d % 20.8f % 20.8f % 20.8f % 20.8f\n", sys->nfi, sys->temp, sys->ekin, sys->epot, sys->ekin+sys->epot);
    fprintf(erg,"% 8d % 20.8f % 20.8f % 20.8f % 20.8f\n", sys->nfi, sys->temp, sys->ekin, sys->epot, sys->ekin+sys->epot);
    fprintf(traj,"%d\n nfi=%d etot=%20.8f\n", sys->natoms, sys->nfi, sys->ekin+sys->epot);
    for (i=0; i<sys->natoms; ++i) {
        fprintf(traj, "Ar  %20.8f %20.8f %20.8f\n", sys->rx[i], sys->ry[i], sys->rz[i]);
    }
}


/* main */
int main(int argc, char **argv){
    int nprint, i;
    char restfile[BLEN], trajfile[BLEN], ergfile[BLEN], line[BLEN];
    FILE *fp,*traj,*erg;
    mdsys_t sys;
    double t_start;

    printf("LJMD version %3.1f\n", LJMD_VERSION);

    t_start = wallclock();

    /* read input file */
    if(get_a_line(stdin,line)) return 1;
    // Convert string integer representation in integer
    sys.natoms=atoi(line);
    if(get_a_line(stdin,line)) return 1;
    /* converts a string representation of a floating-point
    * number to its corresponding double value
    */
    sys.mass=atof(line);
    if(get_a_line(stdin,line)) return 1;
    sys.epsilon=atof(line);
    if(get_a_line(stdin,line)) return 1;
    sys.sigma=atof(line);
    if(get_a_line(stdin,line)) return 1;
    sys.rcut=atof(line);
    if(get_a_line(stdin,line)) return 1;
    sys.box=atof(line);
    if(get_a_line(stdin,restfile)) return 1;
    if(get_a_line(stdin,trajfile)) return 1;
    if(get_a_line(stdin,ergfile)) return 1;
    if(get_a_line(stdin,line)) return 1;
    sys.nsteps=atoi(line);
    if(get_a_line(stdin,line)) return 1;
    sys.dt=atof(line);
    if(get_a_line(stdin,line)) return 1;
    nprint=atoi(line);

    /* allocate memory */
    sys.rx=(double *)malloc(sys.natoms*sizeof(double));
    sys.ry=(double *)malloc(sys.natoms*sizeof(double));
    sys.rz=(double *)malloc(sys.natoms*sizeof(double));
    sys.vx=(double *)malloc(sys.natoms*sizeof(double));
    sys.vy=(double *)malloc(sys.natoms*sizeof(double));
    sys.vz=(double *)malloc(sys.natoms*sizeof(double));
    sys.fx=(double *)malloc(sys.natoms*sizeof(double));
    sys.fy=(double *)malloc(sys.natoms*sizeof(double));
    sys.fz=(double *)malloc(sys.natoms*sizeof(double));

    /* read restart */
    fp=fopen(restfile,"r");
    if(fp) {
        for (i=0; i<sys.natoms; ++i) {
            fscanf(fp,"%lf%lf%lf",sys.rx+i, sys.ry+i, sys.rz+i);
        }
        for (i=0; i<sys.natoms; ++i) {
            fscanf(fp,"%lf%lf%lf",sys.vx+i, sys.vy+i, sys.vz+i);
        }
        fclose(fp);
        azzero(sys.fx, sys.natoms);
        azzero(sys.fy, sys.natoms);
        azzero(sys.fz, sys.natoms);
    } else {
        perror("cannot read restart file");
        return 3;
    }

    /* initialize forces and energies.*/
    sys.nfi=0;
    force(&sys);
    ekin(&sys);

    erg=fopen(ergfile,"w");
    traj=fopen(trajfile,"w");

    printf("Startup time: %10.3fs\n", wallclock()-t_start);
    printf("Starting simulation with %d atoms for %d steps.\n",sys.natoms, sys.nsteps);
    printf("     NFI            TEMP            EKIN                 EPOT              ETOT\n");
    output(&sys, erg, traj);

    /* reset timer */
    t_start = wallclock();

    /**************************************************/
    /* main MD loop */
    for(sys.nfi=1; sys.nfi <= sys.nsteps; ++sys.nfi) {
        /* write output, if requested */
        if ((sys.nfi % nprint) == 0){
            output(&sys, erg, traj);
        }
        /* propagate system and recompute energies */
        velverlet(&sys);
        ekin(&sys);
    }
    /**************************************************/

    /* clean up: close files, free memory */
    printf("Simulation Done. Run time: %10.3fs\n", wallclock()-t_start);
    fclose(erg);
    fclose(traj);

    free(sys.rx);
    free(sys.ry);
    free(sys.rz);
    free(sys.vx);
    free(sys.vy);
    free(sys.vz);
    free(sys.fx);
    free(sys.fy);
    free(sys.fz);

    return 0;
}
