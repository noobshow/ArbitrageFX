#ifndef EXECUTIONTIMER_H
#define EXECUTIONTIMER_H

#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <iomanip>

#include "common/loguru.hpp"

using namespace std::chrono;

/**
 * Armazena a soma do tempo das ultimas execucoes.
 */
static double sumExecutionTimer = 0.0;
/**
 * Contador das execucoes.
 */
static double countExecutionTimer = 0.0;

/**
 * \class ExecutionTimer
 * \brief Classe responsável por calcular o tempo de execução
 * Contador é iniciado na sua criacao e finalizado no destrutor
 */
class ExecutionTimer
{
private:
    /**
     * Nome da funcao ou identificador do escopo.
     */
    std::string name;
    /**
     * Tempo minimo de execucao para logar .
     */
    double minExecutionTimer;
    /**
     * Guarda o time point do inicio da execucao.
     */
    const steady_clock::time_point mStart = steady_clock::now();

public:
    /**
     * Construtor da classe.
     * \param name identificador da execucao.
     * \param minExecTimer tempo de execucao minimo para logar.
     * \param logLevel level do log.
     */
    ExecutionTimer(std::string name = "", double minExecTimer = 0.0)
        : name(name),
          minExecutionTimer(minExecTimer)
    {
    }
    /**
     * Destrutor da classe.
     */
    ~ExecutionTimer()
    {
        // calcula tempo de execucao, end time_point - start time_point
        double timeElapsed =  duration<double, std::nano> (steady_clock::now() - mStart).count();

        // converte nano para segundos
        timeElapsed = timeElapsed / 1000000000;

        std::stringstream ss;

        if(this->minExecutionTimer > 0.0)
        {
            sumExecutionTimer += timeElapsed;
            countExecutionTimer += 1.0;

            // Loga somente quando o tempo de execucao estiver maior que o definido
            if(timeElapsed >= minExecutionTimer)
            {
                double avgExecTime = sumExecutionTimer / countExecutionTimer;
                ss << this->name << " -> Tempo de execucao maior que o limite: "
                   << std::fixed << std::setprecision(9) << timeElapsed
                   << "s, Tempo medio: " << avgExecTime << "s";
                LOG_INFO << ss.str();

            }
            // limita contador para ser usado no processamento de amostras do laser
            if(countExecutionTimer > 1800000.0)
            {
                sumExecutionTimer = sumExecutionTimer / 2.0;
                countExecutionTimer = countExecutionTimer / 2.0;
            }
        }
        else{
            ss << this->name << " -> Tempo de execucao: " << std::fixed << std::setprecision(9) << timeElapsed << "s";
            LOG_INFO << ss.str();
        }
    }
};

#endif // EXECUTIONTIMER_H
