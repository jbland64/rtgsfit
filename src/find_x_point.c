#include "find_x_point.h"
#include <stdio.h>
#include <math.h>
#include "constants.h"
#include <float.h>
#include "gradient.h"


float lin_intrp(
        double* psi,
        int idx, 
        double dist_to_null_r, 
        double dist_to_null_z,
        double abs_dist_null_r,
        double abs_dist_null_z
        )
{
double psi_up, psi_down, psi_at_null;

abs_dist_null_r = abs_dist_null_r / DR;
abs_dist_null_z = abs_dist_null_z / DZ;

if (dist_to_null_r > 0.0)
    {
        if (dist_to_null_z > 0.0)
        {
            psi_up = (1 - abs_dist_null_r)*psi[idx + N_R] + abs_dist_null_r*psi[idx + N_R + 1];
            psi_down = (1 - abs_dist_null_r)*psi[idx] + abs_dist_null_r*psi[idx + 1];
            psi_at_null = (1 - abs_dist_null_z)*psi_down + abs_dist_null_z*psi_up;
        }
        else
        {
            psi_up = (1 - abs_dist_null_r)*psi[idx] + abs_dist_null_r*psi[idx + 1];
            psi_down = (1 - abs_dist_null_r)*psi[idx - N_R] + abs_dist_null_r*psi[idx - N_R + 1];
            psi_at_null = (1 - abs_dist_null_z)*psi_up + abs_dist_null_z*psi_down;
        }
    }
    else
    {
        if (dist_to_null_z > 0.0)
        {
            psi_up = (1 - abs_dist_null_r)*psi[idx + N_R] + abs_dist_null_r*psi[idx + N_R - 1];
            psi_down = (1 - abs_dist_null_r)*psi[idx] + abs_dist_null_r*psi[idx - 1];
            psi_at_null = (1 - abs_dist_null_z)*psi_down + abs_dist_null_z*psi_up;
        
        }
        else
        {
            psi_up = (1 - abs_dist_null_r)*psi[idx] + abs_dist_null_r*psi[idx - 1];
            psi_down = (1 - abs_dist_null_r)*psi[idx - N_R] + abs_dist_null_r*psi[idx - N_R - 1];
            psi_at_null = (1 - abs_dist_null_z)*psi_up + abs_dist_null_z*psi_down;                
        }
    }
    return psi_at_null;
}


void find_null_in_gradient(
        double* flux,
        double* opt_r,
        double* opt_z,
        double* opt_flux,
        int* opt_n,
        double* xpt_r,
        double* xpt_z,
        double* xpt_flux,
        int* xpt_n       
        )
{
    
    int idx, i_row, i_col;
    double hess_det;
    double dist_to_null_r, dist_to_null_z, abs_dist_null_r, abs_dist_null_z;
    double grad_z[N_GRID], grad_r[N_GRID], hess_zz[N_GRID], hess_rr[N_GRID], hess_rz[N_GRID];
    double hess_det_at_null;
    double hess_rr_at_null;
    double hess_zz_at_null;
    double hess_rz_at_null;    
    
    *opt_n = 0;
    *xpt_n = 0;
    
    // find hessian & gradients 
    gradient_z(flux, grad_z);   
    gradient_r(flux, grad_r);
    hessian_zz(flux, hess_zz);
    hessian_rr(flux, hess_rr);
    gradient_r(grad_z, hess_rz);
    
    for (i_row=1; i_row<(N_Z-1); i_row++) 
    {
        for (i_col=1; i_col<(N_R-1); i_col++)
        {
            
            idx = i_row*N_R + i_col;
            hess_det = hess_zz[idx]*hess_rr[idx] - hess_rz[idx]*hess_rz[idx];
            dist_to_null_r = (grad_z[idx]*hess_rz[idx] - hess_zz[idx]*grad_r[idx])/hess_det;
            dist_to_null_z = (grad_r[idx]*hess_rz[idx] - hess_rr[idx]*grad_z[idx])/hess_det;
            
            abs_dist_null_r = fabs(dist_to_null_r);
            abs_dist_null_z = fabs(dist_to_null_z);
            
            if (abs_dist_null_r < 0.5*DR && abs_dist_null_z < 0.5*DZ && MASK_LIM[idx])
            {   
            
                hess_rr_at_null = lin_intrp(hess_rr, idx, dist_to_null_r, 
                            dist_to_null_z, abs_dist_null_r, abs_dist_null_z);
                hess_zz_at_null = lin_intrp(hess_zz, idx, dist_to_null_r, 
                            dist_to_null_z, abs_dist_null_r, abs_dist_null_z);
                hess_rz_at_null = lin_intrp(hess_rz, idx, dist_to_null_r, 
                            dist_to_null_z, abs_dist_null_r, abs_dist_null_z);  
                            
                hess_det_at_null = hess_rr_at_null*hess_zz_at_null - \
                        hess_rz_at_null*hess_rz_at_null;
                                                                                       
                if (hess_det_at_null > 0.0 && hess_rr_at_null < 0.0) 
                {
                    opt_r[*opt_n] = R_VEC[i_col] + dist_to_null_r;
                    opt_z[*opt_n] = Z_VEC[i_row] + dist_to_null_z;    
                    opt_flux[*opt_n] = lin_intrp(flux, idx, dist_to_null_r, 
                            dist_to_null_z,  abs_dist_null_r, abs_dist_null_z);
                    *opt_n += 1;
                }
                else if (hess_det_at_null < 0.0)
                {
                    xpt_r[*xpt_n] = R_VEC[i_col] + dist_to_null_r;
                    xpt_z[*xpt_n] = Z_VEC[i_row] + dist_to_null_z;    
                    xpt_flux[*xpt_n] = lin_intrp(flux, idx, dist_to_null_r, 
                            dist_to_null_z, abs_dist_null_r, abs_dist_null_z);
                    *xpt_n += 1;     
                }
            }
        }
    }
}


       
void find_lcfs_rz(
        double* flux_orig,
        double flux_lcfs,
        double* lcfs_r,
        double* lcfs_z,
        int* lcfs_n
        )
{
    
    int i_row, i_col, idx;
    double off;
    double flux[N_GRID];
    
    *lcfs_n = 0;
    
    for (idx=0; idx<(N_Z*N_R); idx++)
    {
        flux[idx] = flux_orig[idx] - flux_lcfs;
    }
    
    for (i_row=0; i_row<N_Z-1; i_row++)
    {
        for (i_col=0; i_col<N_R-1; i_col++)
        {
            idx = i_row*N_R + i_col;
            
            if ((fabs(flux[idx]) < THRESH)  && MASK_LIM[idx])
            {

                lcfs_r[*lcfs_n] = R_VEC[i_col];
                lcfs_z[*lcfs_n] = Z_VEC[i_row];
                *lcfs_n = *lcfs_n + 1;
            }
            else 
            {
                if ((flux[idx] * flux[idx+1] < 0)  && (MASK_LIM[idx] || MASK_LIM[idx+1]))
                {
                    off = fabs(flux[idx]) / (fabs(flux[idx]) + fabs(flux[idx+1]));
                    if (((off <= 0.5) && MASK_LIM[idx]) || ((off >= 0.5) && MASK_LIM[idx+1]))
                    {
                        lcfs_r[*lcfs_n] = R_VEC[i_col] + off*DR;
                        lcfs_z[*lcfs_n ] = Z_VEC[i_row];
                        *lcfs_n = *lcfs_n + 1;
                    }
                }
                if ((flux[idx] * flux[idx+N_R] < 0) && (MASK_LIM[idx] || MASK_LIM[idx+N_R]))
                {
                    off = fabs(flux[idx]) / (fabs(flux[idx]) + fabs(flux[idx+N_R]));
                    if (((off <= 0.5) && MASK_LIM[idx]) || ((off >= 0.5) && MASK_LIM[idx+N_R]))
                    {
                        lcfs_r[*lcfs_n] = R_VEC[i_col];
                        lcfs_z[*lcfs_n ] = Z_VEC[i_row] + off*DZ;
                        *lcfs_n = *lcfs_n + 1;
                    }
                }
            }            
         
        }
    }
    
    for (i_row=0; i_row<N_Z-1; i_row++)
    {
        idx = i_row*N_R + N_R - 1;
        
        if ((fabs(flux[idx]) < THRESH) && MASK_LIM[idx])
        {

            lcfs_r[*lcfs_n] = R_VEC[i_col];
            lcfs_z[*lcfs_n] = Z_VEC[i_row];
            *lcfs_n = *lcfs_n + 1;
        }
        else if ((flux[idx] * flux[idx+N_R] < 0) && (MASK_LIM[idx] || MASK_LIM[idx+N_R]))
        {
            off = fabs(flux[idx]) / (fabs(flux[idx]) + fabs(flux[idx+N_R]));
            if (((off <= 0.5) && MASK_LIM[idx]) || ((off >= 0.5) && MASK_LIM[idx+N_R]))
            { 
                lcfs_r[*lcfs_n] = R_VEC[i_col];
                lcfs_z[*lcfs_n ] = Z_VEC[i_row] + off*DZ;
                *lcfs_n = *lcfs_n + 1;
            }
        }            
     
    }
    
    for (i_col=0; i_col<N_R-1; i_col++)
    {
        idx = (N_Z-1)*N_R + i_col;
        
        if ((fabs(flux[idx]) < THRESH) && MASK_LIM[idx])
        {

            lcfs_r[*lcfs_n] = R_VEC[i_col];
            lcfs_z[*lcfs_n] = Z_VEC[i_row];
            *lcfs_n = *lcfs_n + 1;
        }
        else if ((flux[idx] * flux[idx+1] < 0) && (MASK_LIM[idx] || MASK_LIM[idx+1]))
        {
            off = fabs(flux[idx]) / (fabs(flux[idx]) + fabs(flux[idx+1]));
            if (((off <= 0.5) && MASK_LIM[idx]) || ((off >= 0.5) && MASK_LIM[idx+1]))
            {
                lcfs_r[*lcfs_n] = R_VEC[i_col] + off*DR;
                lcfs_z[*lcfs_n ] = Z_VEC[i_row];
                *lcfs_n = *lcfs_n + 1;
            }
        }
    }
} 
       
              
void inside_lcfs(
        double r_opt,
        double z_opt,
        double* lcfs_r,
        double* lcfs_z,
        int lcfs_n, 
        int* mask
        )        
{
    
    int i_row, i_col, i_lcfs, col_start, col_end, row_start, row_end, i_grid, i_count;
    int count = 0;
    double z_nearest;
    double r_start, r_end;
    double r_tmp[N_XPT_MAX];
    double z_start, z_end;
    double z_tmp[N_XPT_MAX];

    for (i_grid=0; i_grid<N_GRID; i_grid++)
    {
        mask[i_grid] = 0 ;
    }

    z_nearest = round((z_opt - Z_VEC[0])/DZ)*DZ + Z_VEC[0];
    
    for (i_lcfs=0; i_lcfs<lcfs_n; i_lcfs++)
    {
        if (fabs(lcfs_z[i_lcfs] - z_nearest) < THRESH)
        {
            r_tmp[count] = lcfs_r[i_lcfs];
            count += 1;
        }
    }
    r_start = -DBL_MAX;
    r_end = DBL_MAX;
    
    for (i_count=0; i_count< count; i_count++)
    {
        if (r_tmp[i_count] < r_opt)
        {
            if (r_tmp[i_count] > r_start)
            {
                r_start = r_tmp[i_count];
            }
        }
        else 
        {
            if (r_tmp[i_count] < r_end)
            {
                r_end = r_tmp[i_count];
            }
        }
    }

    col_start = (int) ceil((r_start - R_VEC[0] ) / DR);
    col_end = (int) floor((r_end - R_VEC[0])/DR);

    for (i_col=col_start; i_col<=col_end; i_col++)
    {
        count = 0;
        for (i_lcfs=0; i_lcfs<lcfs_n; i_lcfs++)
        {
            if (fabs(lcfs_r[i_lcfs] - R_VEC[i_col]) < THRESH)
            {
                z_tmp[count] = lcfs_z[i_lcfs];
                count += 1;
            }
        }

        z_start = -DBL_MAX;
        z_end = DBL_MAX;

        for (i_count=0; i_count< count; i_count++)
        {
            if (z_tmp[i_count] < z_opt)
            {
                if (z_tmp[i_count] > z_start)
                {
                    z_start = z_tmp[i_count];
                }
            }
            else 
            {
                if (z_tmp[i_count] < z_end)
                {
                    z_end = z_tmp[i_count];
                }
            }
        }

        row_start = (int) ceil((z_start - Z_VEC[0] ) / DZ);
        row_end = (int) floor((z_end - Z_VEC[0])/DZ);
        
        for (i_row=row_start; i_row<=row_end; i_row++)
        {
            mask[i_row*N_R + i_col] = 1;
        }
    }
}   
