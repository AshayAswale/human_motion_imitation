#include <tough_common/robot_state.h>
#include <eigen3/Eigen3.3.7/Core>
#include <trajectory_msgs/JointTrajectory.h>

class JointAnglesController
{
private:
  int chest_index_, left_arm_index_, right_arm_index_;
  int chest_joint_number_, left_arm_joint_number_, right_arm_joint_number_;
  int chest_size_, left_arm_size_, right_arm_size_;

  std::vector<std::string> joint_names_;
  std::vector<double> curr_joint_angles_;

  Eigen::DiagonalMatrix<double, Eigen::Dynamic> k_p_, k_d_, derivative_;
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> max_jt_accn, min_jt_accn;
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> p_out_, d_out_, contr_output_;
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> curr_position_, prev_position_, desd_position_;
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> error_, prev_error_;

  size_t total_joints_size_;
  double d_t;

  ros::NodeHandle nh_;
  RobotStateInformer* state_informer_;
  RobotDescription* rd_;

  void initializeVariables();
  void initializeMatrices(const size_t size);

  void updateControlOutput();
  void updateCurrJointAngles();
  void limitAccelerations(Eigen::DiagonalMatrix<double, Eigen::Dynamic>& matrix);
  void printMatrix(Eigen::DiagonalMatrix<double, Eigen::Dynamic>& matrix, const std::string& matrix_name);

  int getJointNumber(const std::string& joint_name);

  std::vector<double> diagonalMatrixToVector(const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& matrix);

  /**
   * @brief Inserts values in the matrix
   *
   * @param vector            - Input vector
   * @param start_index       - Index of the first element which should go into matrix
   * @param end_index         - Index of the element next to the last element. (or start_index + size)
   * @param matrix            - [output]
   */
  inline void insertValuesInMatrix(const std::vector<double>& vector, const int vec_start_index, const int vec_end_index, int mat_start_index,
                                   Eigen::DiagonalMatrix<double, Eigen::Dynamic>& matrix)
  {
    for (int i = vec_start_index; i < vec_end_index; i++, mat_start_index++)
    {
      matrix.diagonal()(mat_start_index) = vector.at(i);
    }
  }

  /*
      SHOULD BE IMPLEMENTED USING THE RETURN VALUE OPTIMISATION, NOT CALL BY REF.
      THIS IS ONLY TEMPORARY FIX
  */
  inline void vectorToDiagonalMatrix(const std::vector<double>& vector,
                                     Eigen::DiagonalMatrix<double, Eigen::Dynamic>& matrix)
  {
    int size = vector.size();

    for (int i = 0; i < vector.size(); i++)
    {
      matrix.diagonal()(i) = vector.at(i);
    }
  }

public:
  JointAnglesController(ros::NodeHandle nh);
  ~JointAnglesController();

  /**
   * @brief Get the Controlled Joint Angles object
   *
   * @param joint_angles            - Vector of the desired positions for the chest, left arm and right arm.
   *                                  Indices for the start of the joints should be inquired through getters.
   * @return std::vector<double>    - Returns the vector of accelerations
   */
  std::vector<double> getControlledJointAngles(const std::vector<double>& joint_angles);

  /**
   * @brief Updates the Acceleration parameter in the trajectory message. Takes position vector from message
   *        as input and updates the accelerations accordingly.
   *
   * @param traj_msg                - Acceleration is updated with the help of position of this message.
   */
  void updateJointAccelerations(trajectory_msgs::JointTrajectory& traj_msg);

  void getKp(const int joint_number) const;
  void getKd(const int joint_number) const;

  void setKp(const double Kp, const int joint_number);
  void setKd(const double Kd, const int joint_number);

  std::vector<double> getJointsKp() const;
  std::vector<double> getJointsKd() const;

  void setJointsKp(const std::vector<double>& joints_kp);
  void setJointsKd(const std::vector<double>& joints_kd);

  std::vector<double> getDefaultGains();
  void setDefaultGains();

  int getChestIndexAcceleration() const;
  int getLeftArmIndexAcceleration() const;
  int getRightArmIndexAcceleration() const;
};

Eigen::DiagonalMatrix<double, Eigen::Dynamic> operator-(const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_1,
                                                        const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_2)
{
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> output_mat;
  output_mat.diagonal() = mat_1.diagonal() - mat_2.diagonal();
  return output_mat;
}

Eigen::DiagonalMatrix<double, Eigen::Dynamic> operator+(const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_1,
                                                        const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_2)
{
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> output_mat;
  output_mat.diagonal() = mat_1.diagonal() + mat_2.diagonal();
  return output_mat;
}

Eigen::DiagonalMatrix<double, Eigen::Dynamic> operator*(const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_1,
                                                        const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_2)
{
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> output_mat;
  output_mat.diagonal() = mat_1.toDenseMatrix() * mat_2.diagonal();
  return output_mat;
}

Eigen::DiagonalMatrix<double, Eigen::Dynamic> operator/(const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& mat_1,
                                                        const int scaler)
{
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> output_mat;
  output_mat.diagonal() = mat_1.diagonal() * (1/scaler);
  return output_mat;
}