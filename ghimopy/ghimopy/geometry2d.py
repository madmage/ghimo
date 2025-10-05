import math


def angle_diff(a0, a1):
    return math.atan2(math.sin(a0 - a1), math.cos(a0 - a1))


def pose_is_close(pose1, pose2, dist_tolerance=0.1, angle_tolerance=0.1):        
    return (                                                                     
        math.hypot(pose2[0] - pose1[0], pose2[1] - pose1[1]) < dist_tolerance    
        and math.fabs(angle_diff(pose2[2], pose1[2])) < angle_tolerance          
    )                                                                            
