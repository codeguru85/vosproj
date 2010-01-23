package util.page
{
	import mx.collections.ArrayCollection;
	
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.web.Finder")]
	public class Finder
	{
		public var list:ArrayCollection;

		public var totalRows:int;
	
		public var totalPages:int; 
	
		public var pageSize:int;
	
		public var currentPage:int;
	
		public var toPage:int; 
	
		public var hasPrevious:Boolean;
	
		public var hasNext:Boolean;
		
		public function Finder():void{
			
		}
	}
}