package clases;

import java.util.Objects;

public class VCalidadAgua {
	
	private Integer idagua;
	private Integer idSensorcad;
	private Double calidadAgua;
	private Long Mtemporalcad;

	public VCalidadAgua(Integer idagua,Integer idSensorcad, Double calidadAgua, Long Mtemporalcad) {
		super();
		this.idagua=idagua;
		this.idSensorcad = idSensorcad;
		this.calidadAgua = calidadAgua;
		this.Mtemporalcad = Mtemporalcad;

	}
	public VCalidadAgua() {
		super();
	}
	public Integer getIdagua() {
		return idagua;
	}
	public void setIdagua(Integer idagua) {
		this.idagua = idagua;
	}
	public Integer getIdSensorcad() {
		return idSensorcad;
	}
	public void setIdSensorcad(Integer idSensorcad) {
		this.idSensorcad = idSensorcad;
	}
	public Double getCalidadAgua() {
		return calidadAgua;
	}
	public void setCalidadAgua(Double calidadAgua) {
		this.calidadAgua = calidadAgua;
	}
	public Long getMtemporalcad() {
		return Mtemporalcad;
	}
	public void setMtemporalcad(Long mtemporalcad) {
		Mtemporalcad = mtemporalcad;
	}
	@Override
	public int hashCode() {
		return Objects.hash(Mtemporalcad, calidadAgua, idSensorcad, idagua);
	}
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		VCalidadAgua other = (VCalidadAgua) obj;
		return Objects.equals(Mtemporalcad, other.Mtemporalcad) && Objects.equals(calidadAgua, other.calidadAgua)
				&& Objects.equals(idSensorcad, other.idSensorcad) && Objects.equals(idagua, other.idagua);
	}
	@Override
	public String toString() {
		return "VCalidadAgua [idagua=" + idagua + ", idSensorcad=" + idSensorcad + ", calidadAgua=" + calidadAgua
				+ ", Mtemporalcad=" + Mtemporalcad + "]";
	}
	
	
	
	
	
	

}
